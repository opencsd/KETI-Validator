// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory)
#include "scan.h"

using namespace ROCKSDB_NAMESPACE;

int temp = 0; //kjh test
int current_row_count = 0; //kjh test

char sep = 0x03;
char gap = 0x20;
char fin = 0x02;

inline Slice ExtractUserKey(const Slice& internal_key, uint64_t kNumInternalBytes) {
  assert(internal_key.size() >= kNumInternalBytes);
  return Slice(internal_key.data(), internal_key.size() - kNumInternalBytes);
}

class InternalKey {
 private:
  string rep_;
  uint64_t kNumInternalBytes_;

 public:
  InternalKey() {}  // Leave rep_ as empty to indicate it is invalid
  void DecodeFrom(const Slice& s, uint64_t k) { 
    rep_.assign(s.data(), s.size()); 
    kNumInternalBytes_ = k;
  }
  Slice user_key() const { return ExtractUserKey(rep_, kNumInternalBytes_); }
};

void Scan::Scanning(){
    while (1){
        Snippet snippet = ScanQueue.wait_and_pop();

        TableRep table_rep = CSDTableManager::GetTableRep(snippet.table_name);
        kNumInternalBytes_ = snippet.kNumInternalBytes;
		                
        Options options;
        SstBlockReader sstBlockReader(
            options, table_rep.blocks_maybe_compressed, table_rep.blocks_definitely_zstd_compressed, 
            table_rep.immortal_table, table_rep.read_amp_bytes_per_bit, table_rep.dev_name);

        FilterInfo filterInfo(
          snippet.table_col, snippet.table_offset, snippet.table_offlen,
          snippet.table_datatype, snippet.colindexmap, snippet.table_filter,
          snippet.column_projection, snippet.projection_datatype, snippet.projection_length);

        Result scanResult(snippet.query_id, snippet.work_id, snippet.csd_name, snippet.total_block_count, filterInfo);
        
        current_block_count = 0;
        index_valid = true;
        ipk = 0;
        check = true;
        total_block_row_count = 0;
        temp = 0; //kjh test

        // if(snippet.is_inserted){
        //   WalScan(&snippet, &scanResult);
        //   EnQueueData(scanResult, snippet);
        //   scanResult.InitResult();
        // }

        list<BlockInfo>::iterator iter;
        for(iter = snippet.block_info_list.begin(); iter != snippet.block_info_list.end(); iter++){//블록
            current_block_count++;
            current_row_count = 0;//kjh test
            
            scanResult.result_block_count++;

            BlockInfo blockInfo = *iter;
            BlockScan(&sstBlockReader, &blockInfo, &snippet, &scanResult);

            if(!index_valid){
              scanResult.result_block_count += snippet.total_block_count - current_block_count;
              current_block_count = snippet.total_block_count;
            }

            if(current_block_count == snippet.total_block_count){
              float temp_size = float(scanResult.length) / float(1024);
              memset(msg, '\0', sizeof(msg));
              // sprintf(msg,"Scanning Data ... (Block : %d/%d, Scanned Size : %.1fK, Total Rows: %d, BLock Rows: %d)\n",current_block_count,snippet.total_block_count,temp_size, total_block_row_count, temp);
              sprintf(msg,"Scanning Data ... (Block : %d/%d, Scanned Size : %.1fK, Total Rows: %d)\n",current_block_count,snippet.total_block_count,temp_size, total_block_row_count);
              KETILOG::INFOLOG(LOGTAG, msg);
              EnQueueData(scanResult, snippet);
              scanResult.InitResult();
              break;
            }else if(current_block_count % NUM_OF_BLOCKS == 0){
              float temp_size = float(scanResult.length) / float(1024);
              memset(msg, '\0', sizeof(msg));
              // sprintf(msg,"Scanning Data ... (Block : %d/%d, Scanned Size : %.1fK, Blcok Rows: %d)\n",current_block_count,snippet.total_block_count,temp_size, temp);
              sprintf(msg,"Scanning Data ... (Block : %d/%d, Scanned Size : %.1fK)\n",current_block_count,snippet.total_block_count,temp_size);
              KETILOG::DEBUGLOG(LOGTAG, msg);
              
              temp = 0;//kjh
              EnQueueData(scanResult, snippet);
              scanResult.InitResult();
            }

            memset(msg, '\0', sizeof(msg));
            sprintf(msg,"current block count : %d | current row count : %d\n", current_block_count, current_row_count);//kjh test
            KETILOG::TRACELOG(LOGTAG, msg);
        }
    }
}

void Scan::WalScan(Snippet *snippet_, Result *scan_result){
  for(int i=0; i<snippet_->inserted_key.size(); i++){
      scan_result->row_offset.push_back(scan_result->length);
      int row_size = 0;
      char* inserted_data;
      inserted_data = hexstr_to_char(snippet_->inserted_value[i].c_str(),&row_size);
      memcpy(scan_result->data+scan_result->length,inserted_data,row_size);
      scan_result->length += row_size;
      scan_result->row_count++;
  }
}

void Scan::BlockScan(SstBlockReader* sstBlockReader_, BlockInfo* blockInfo, Snippet *snippet_, Result *scan_result){
  Status s  = sstBlockReader_->Open(blockInfo);
  if(!s.ok()){
      KETILOG::ERRORLOG(LOGTAG, "Block Open Error \n");
  }

  const char* ikey_data;
  const char* row_data;
  size_t row_size;

  Iterator* datablock_iter = sstBlockReader_->NewIterator(ReadOptions());

  if(snippet_->scan_type == Full_Scan_Filter || snippet_->scan_type == Full_Scan){//full table scan

    for (datablock_iter->SeekToFirst(); datablock_iter->Valid(); datablock_iter->Next()) {//iterator first부터 순회
      
      Status s = datablock_iter->status();
      if (!s.ok()) {
        KETILOG::ERRORLOG(LOGTAG, "Error reading the block - Skipped \n");
        break;
      }               

      const Slice& key = datablock_iter->key();
      const Slice& value = datablock_iter->value();

      InternalKey ikey;
      ikey.DecodeFrom(key, snippet_->kNumInternalBytes);

      ikey_data = ikey.user_key().data();
      row_data = value.data();
      row_size = value.size();
      
      if(check){//index num 획득 임시 추가(나중엔 스니펫이 주기)
        memcpy(origin_index_num,ikey_data,INDEX_NUM_SIZE);
        check = false;
      }

      //check row index number
      char index_num[INDEX_NUM_SIZE];
      memcpy(index_num,ikey_data,INDEX_NUM_SIZE);
      if(memcmp(origin_index_num/*snippet_->index_num*/, index_num, INDEX_NUM_SIZE) != 0){//출력 지우지 말기
        KETILOG::WARNLOG(LOGTAG, "index invalid \n");
        index_valid = false;
        return;
      }

      // //check is deleted key
      // const char *key_data = key.data();
      // string keystr = char_to_hexstr(key_data,kNumInternalBytes_+indexnum_size);
      // if(!(find(snippet_->deleted_key.begin(), snippet_->deleted_key.end(), keystr) == snippet_->deleted_key.end())) { 
      //   cout << "deleted" << endl;
      //   continue;
      // }
      
      // std::cout << "[Row(HEX)] KEY: " << ikey.user_key().ToString(true) << " | VALUE: " << value.ToString(true) << endl;

      scan_result->row_offset.push_back(scan_result->length);
      total_block_row_count++;
      current_row_count++;
      temp++;

      if(snippet_->primary_length != 0){//pk있으면 붙이기
        char total_row_data[snippet_->primary_length+row_size];
        int pk_length;

        pk_length = getPrimaryKeyData(ikey_data, total_row_data, snippet_->primary_key_list);//key
       
        memcpy(total_row_data + pk_length, row_data, row_size);//key+value
        memcpy(scan_result->data + scan_result->length, total_row_data, pk_length + row_size);//buff+key+value
        
        scan_result->length += row_size + pk_length;
        scan_result->row_count++;
      }else{//없으면 value만 붙이기
        memcpy(scan_result->data+scan_result->length, row_data, row_size);
        scan_result->length += row_size;
        scan_result->row_count++; 
      }
    } 

  }else{//index scan
    // string pk_str = snippet_->index_pk;
    // Document document;
    // document.Parse(pk_str.c_str());
    // Value &index_pk = document["index_pk"];

    // vector<char> target_pk;
    // target_pk.assign(snippet_->index_num,snippet_->index_num+4);

    // bool pk_valid = true;

    // while(pk_valid){
    //   for(int i=0; i<index_pk.Size(); i++){
    //     int key_type = snippet_->table_datatype[i];

    //     if(key_type == MySQL_INT32 || key_type == MySQL_DATE){//int(int, date)
    //       int key_length = snippet_->table_offlen[i];
    //       union{
    //         int value;
    //         char byte[4];
    //       }pk;
    //       pk.value = index_pk[i][ipk].GetInt();
    //       for(int j=0; j<key_length; j++){
    //         target_pk.push_back(pk.byte[j]);
    //       }

    //     }else if(key_type == MySQL_INT64 || key_type == 246){//int64_t(bigint, decimal)
    //       int key_length = snippet_->table_offlen[i];
    //       union{
    //         int64_t value;
    //         char byte[8];
    //       }pk;
    //       pk.value = index_pk[i][ipk].GetInt64();
    //       for(int j=0; j<key_length; j++){
    //         target_pk.push_back(pk.byte[j]);
    //       }

    //     }else if(key_type == 254 || key_type == 15){//string(string, varstring)
    //       string pk = index_pk[i][ipk].GetString();
    //       int key_length = pk.length();
    //       for(int j=0; j<key_length; j++){
    //         target_pk.push_back(pk[j]);
    //       }
    //     }
    //   }

    //   char *p = &*target_pk.begin();
    //   Slice target_slice(p,target_pk.size());

    //   datablock_iter->Seek(target_slice);

    //   if(datablock_iter->Valid()){//target과 pk가 같은지 한번더 비교
    //     const Slice& key = datablock_iter->key();
    //     const Slice& value = datablock_iter->value();

    //     InternalKey ikey;
    //     ikey.DecodeFrom(key);
    //     ikey_data = ikey.user_key().data();

    //     //테스트 출력
    //     std::cout << "[Row(HEX)] KEY: " << ikey.user_key().ToString(true) << " | VALUE: " << value.ToString(true) << endl;

    //     if(target_slice.compare(ikey.user_key()) == 0){ //target O
    //       row_data = value.data();
    //       row_size = value.size();

    //       char total_row_data[snippet_->primary_length+row_size];
    //       int pk_length;

    //       pk_length = getPrimaryKeyData(ikey_data, total_row_data, snippet_->primary_key_list);//key
        
    //       memcpy(total_row_data + pk_length, row_data, row_size);//key+value
    //       memcpy(scan_result->data + scan_result->length, total_row_data, pk_length + row_size);//buff+key+value
          
    //       scan_result->length += row_size + pk_length;
    //       scan_result->row_count++;
          
    //     }else{//target X
    //       //primary key error(출력 지우지 말기)
    //       cout << "primary key error [no primary key!]" << endl;      

    //       cout << "target: ";
    //       for(int i=0; i<target_slice.size(); i++){
    //         printf("%02X ",(u_char)target_slice[i]);
    //       }
    //       cout << endl;

    //       cout << "ikey: ";
    //       for(int i=0; i<ikey.user_key().size(); i++){
    //         printf("%02X ",(u_char)ikey.user_key()[i]);
    //       }
    //       cout << endl;

    //       //check row index number
    //       char index_num[indexnum_size];
    //       memcpy(index_num,ikey_data,indexnum_size);
    //       if(memcmp(snippet_->index_num, index_num, indexnum_size) != 0){//출력 지우지 말기
    //         cout << "different index number: ";
    //         for(int i=0; i<indexnum_size; i++){
    //           printf("(%02X %02X)",(u_char)snippet_->index_num[i],(u_char)index_num[i]);
    //         }
    //         cout << endl;
    //         index_valid = false;
    //         pk_valid = false;
    //       }
    //     }
    //     ipk++;

    //   }else{
    //     //go to next block
    //     pk_valid = false;
    //   }

    // }
  }
}

void Scan::EnQueueData(Result scan_result, Snippet snippet_){
    if(snippet_.scan_type == Full_Scan_Filter){
      /*if(scan_result.length != 0){//scan->filter*/ //data length 체크하는거 생략!
        FilterQueue.push_work(scan_result);        
      /*}else{//scan->merge
        MergeQueue.push_work(scan_result);
      }*/

    }else if(snippet_.scan_type == Full_Scan){//scan->merge
      MergeQueue.push_work(scan_result);

    }else if(snippet_.scan_type == Index_Scan_Filter){//scan->filter
      if(scan_result.length != 0){
        FilterQueue.push_work(scan_result);
      }else{
        MergeQueue.push_work(scan_result);
      }
      
    }else{//Index_Scan, scan->merge
      MergeQueue.push_work(scan_result);
    }
}

int Scan::getPrimaryKeyData(const char* ikey_data, char* dest, list<PrimaryKey> pk_list){
  int offset = 4;
  int pk_length = 0;

  std::list<PrimaryKey>::iterator piter;
  for(piter = pk_list.begin(); piter != pk_list.end(); piter++){
      int key_length = (*piter).key_length;
      int key_type = (*piter).key_type;

      switch(key_type){
        case MySQL_INT32:
        case MySQL_INT64:{
          char pk[key_length];
          pk[0] = 0x00;//ikey[80 00 00 00 00 00 00 01]->ikey[00 00 00 00 00 00 00 01]
          for(int i = 0; i < key_length; i++){
            pk[i] = ikey_data[offset+key_length-i-1];
            //ikey[00 00 00 00 00 00 00 01]->dest[01 00 00 00 00 00 00 00]
          }
          memcpy(dest+pk_length, pk, key_length);
          pk_length += key_length;
          offset += key_length;
          break;
        }case MySQL_DATE:{
          char pk[key_length];
          for(int i = 0; i < key_length; i++){
            pk[i] = ikey_data[offset+key_length-i-1];
            //ikey[0F 98 8C]->dest[8C 98 0F]
          }
          memcpy(dest+pk_length, pk, key_length);
          pk_length += key_length;
          offset += key_length;
          break;
        }case MySQL_NEWDECIMAL:
         case MySQL_STRING:{
          //ikey_data[39 33 37 2D 32 34 31 2D 33 31 39 38 20 20 20]
          //dest[39 33 37 2D 32 34 31 2D 33 31 39 38 20 20 20]
          memcpy(dest+pk_length, ikey_data+offset, key_length);
          pk_length += key_length;
          offset += key_length;
          break;
        }case MySQL_VARSTRING:{
          char pk[key_length];
          int var_key_length = 0;
          bool end = false;
          /*ikey_data[33 34 35 33 31 32 38 35 {03} 33 34 36 (20 20 20 20 20) {02}]
            dest[{0B} 33 34 35 33 31 32 38 35 33 34 36]*/
          while(!end){
            if(ikey_data[offset] == sep || ikey_data[offset] == gap){//03 or 20
              offset++;
            }else if(ikey_data[offset] == fin){//02
              offset++;   
              end = true;  
            }else{
              pk[var_key_length] = ikey_data[offset];
              offset++;
              var_key_length++;
            }
          }
          if(var_key_length < 256){
            char len = (char)var_key_length;
            dest[pk_length] = len;
            pk_length += 1;
          }else{
            char len[2];
            int l1 = var_key_length / 256;
            int l2 = var_key_length % 256;
            len[0] = (char)l1;
            len[1] = (char)l2;
            memcpy(dest+pk_length,len,2);
            pk_length += 2;
          }
          memcpy(dest+pk_length, pk, var_key_length);
          pk_length += var_key_length;
          break;
        }default:{
          KETILOG::FATALLOG(LOGTAG, "Data Type Not Defined! ("+to_string(key_type)+ ")\n");
        }
      }
  }

  return pk_length;
}

char* Scan::hexstr_to_char(const char* hexstr, int* row_size){
    size_t len = strlen(hexstr);
    if(len % 2 != 0){
        return NULL;
    }
    size_t final_len = len / 2;
    char* chrs = (char*)malloc((final_len+1) * sizeof(*chrs));
    for (size_t i=0, j=0; j<final_len; i+=2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i+1] % 32 + 9) % 25;
    chrs[final_len] = '\0';
    *row_size = static_cast<int>(final_len);
    return chrs;
}

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
string Scan::char_to_hexstr(const char *data, int len){
  std::string s(len * 2, ' ');
  for (int i = 0; i < len; ++i) {
    s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
    s[2 * i + 1] = hexmap[data[i] & 0x0F];
  }
  return s;
}
