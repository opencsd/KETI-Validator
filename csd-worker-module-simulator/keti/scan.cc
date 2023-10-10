// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory)
#include "scan.h"

uint64_t kNumInternalBytes_;
const int indexnum_size = 4;
bool index_valid;
bool check;
char origin_index_num[indexnum_size];
int current_block_count;

char sep = 0x03;
char gap = 0x20;
char fin = 0x02;

int row_count; 

int getPrimaryKeyData(const char* ikey_data, char* dest, list<PrimaryKey> pk_list);
char* hexstr_to_char(const char* hexstr, int* row_size);
string char_to_hexstr(const char *data, int len);

inline Slice ExtractUserKey(const Slice& internal_key) {
  assert(internal_key.size() >= kNumInternalBytes_);
  return Slice(internal_key.data(), internal_key.size() - kNumInternalBytes_);
}

class InternalKey {
 private:
  string rep_;

 public:
  InternalKey() {}  // Leave rep_ as empty to indicate it is invalid
  void DecodeFrom(const Slice& s) { rep_.assign(s.data(), s.size()); }
  Slice user_key() const { return ExtractUserKey(rep_); }
};

void Scan::Scanning(){
  // cout << "<-----------  Scan Layer Running...  ----------->\n";
    while (1){
        Snippet snippet = ScanQueue.wait_and_pop();
        
        string file_path = CSDTableManager_.GetTableRep(snippet.csd_name, snippet.table_name);//여기 수정
        cout << "*****file_path: " << file_path << endl;
        kNumInternalBytes_ = snippet.kNumInternalBytes;
		                
        Options options;
        SstFileReader sstFileReader(options);

        FilterInfo filterInfo(
          snippet.table_col, snippet.table_offset, snippet.table_offlen,
          snippet.table_datatype, snippet.colindexmap, snippet.table_filter,
          snippet.column_projection, snippet.projection_datatype, snippet.projection_length
        );
        Result scanResult(snippet.query_id, snippet.work_id, snippet.csd_name, 
          snippet.total_block_count, filterInfo      
        );

        current_block_count = 0;
        row_count = 0;
        check = true;
        index_valid = true;

        // if(snippet.is_inserted){
        //   WalScan(&snippet, &scanResult);
        //   EnQueueData(scanResult, snippet.scan_type);
        //   scanResult.InitResult();
        // }

        BlockScan(&sstFileReader, &snippet, &scanResult, file_path);
        if(!index_valid){
            scanResult.result_block_count += snippet.total_block_count - current_block_count;
            current_block_count = snippet.total_block_count;
        }else{
            float temp_size = float(scanResult.length) / float(1024);
            scanResult.result_block_count = snippet.total_block_count - current_block_count;
            current_block_count += scanResult.result_block_count; 
        }
        printf("[CSD Scan] 1Scanning Data ... (Block : %d/%d)\n",current_block_count,snippet.total_block_count);
        // cout << "*****row_count: " << row_count << endl;
        
        EnQueueData(scanResult, snippet.scan_type);
        scanResult.InitResult();

        
    }
}

void Scan::WalScan(Snippet *snippet_, Result *scan_result){
  for(int i=0; i<snippet_->inserted_key.size(); i++){
      scan_result->row_offset.push_back(scan_result->length);
      int row_size = 0;
      char* inserted_data;
      inserted_data = hexstr_to_char(snippet_->inserted_value[i].c_str(),&row_size);
      // cout << "inserted_data: ";
      // for(int i=0; i<row_size; i++){
      //     printf("%02X ",(u_char)inserted_data[i]);
      // }
      // cout << endl;
      memcpy(scan_result->data+scan_result->length,inserted_data,row_size);
      scan_result->length += row_size;
      scan_result->row_count++;
  }
}

void Scan::BlockScan(SstFileReader* sstFileReader_, Snippet *snippet_, Result *scan_result, string file_path){
  Status s  = sstFileReader_->Open(file_path);
  if(!s.ok()){
      cout << "open error" << endl;
  }

  const char* ikey_data;
  const char* row_data;
  size_t row_size;

  Iterator* datablock_iter = sstFileReader_->NewIterator(ReadOptions());

  if(snippet_->scan_type == Full_Scan_Filter || snippet_->scan_type == Full_Scan){//full table scan

    for (datablock_iter->SeekToFirst(); datablock_iter->Valid(); datablock_iter->Next()) {//iterator first부터 순회

      Status s = datablock_iter->status();
      if (!s.ok()) {
        cout << "Error reading the block - Skipped \n";
        break;
      }               

      const Slice& key = datablock_iter->key();
      const Slice& value = datablock_iter->value();

      InternalKey ikey;
      ikey.DecodeFrom(key);

      ikey_data = ikey.user_key().data();
      row_data = value.data();
      row_size = value.size();
      
      if(check){//index num 획득 임시 추가
        memcpy(origin_index_num,ikey_data,indexnum_size);
        check = false;
      }

      //check row index number
      char index_num[indexnum_size];
      memcpy(index_num,ikey_data,indexnum_size);
      if(memcmp(origin_index_num/*snippet_->index_num*/, index_num, indexnum_size) != 0){//출력 지우지 말기
        cout << "different index number: ";
        for(int i=0; i<indexnum_size; i++){
          printf("(%02X %02X)",(u_char)origin_index_num[i],(u_char)index_num[i]);
        }
        cout << endl;
        index_valid = false;
        return;
      }

      row_count++;

      //check is deleted key
      // const char *key_data = key.data();
      // string keystr = char_to_hexstr(key_data,kNumInternalBytes_+indexnum_size);
      // if(!(find(snippet_->deleted_key.begin(), snippet_->deleted_key.end(), keystr) == snippet_->deleted_key.end())) { 
      //   cout << "deleted" << endl;
      //   continue;
      // }

      // std::cout << "[Row(HEX)] KEY: " << ikey.user_key().ToString(true) << " | VALUE: " << value.ToString(true) << endl;

      if(snippet_->primary_length != 0){ //pk있으면 붙이기
        char total_row_data[snippet_->primary_length+row_size];
        int pk_length;

        pk_length = getPrimaryKeyData(ikey_data, total_row_data, snippet_->primary_key_list);//key
       
        memcpy(total_row_data + pk_length, row_data, row_size);//key+value
        memcpy(scan_result->data + scan_result->length, total_row_data, pk_length + row_size);//buff+key+value
        
        scan_result->length += row_size + pk_length;
        scan_result->row_count++;
        scan_result -> raw_row_count++;
      }else{//없으면 value만 붙이기
          if(scan_result->length + row_size > 54272){
              current_block_count += 15;
              float temp_size = float(scan_result->length) / float(1024);
              scan_result->result_block_count = 15;
              printf("[CSD Scan] Scanning Data ... (Block : %d/%d)\n",current_block_count,snippet_->total_block_count);
              Result scan_result_(scan_result->query_id,scan_result->work_id,scan_result->csd_name,scan_result->total_block_count,
              scan_result->filter_info,scan_result->result_block_count);
              scan_result_.row_count = scan_result->row_count;
              scan_result_.raw_row_count = scan_result -> raw_row_count;
              scan_result_.length = scan_result->length;
              scan_result_.row_offset.assign(scan_result->row_offset.begin(),scan_result->row_offset.end());
              memcpy(scan_result_.data,scan_result->data, scan_result->length);

              EnQueueData(scan_result_, snippet_->scan_type);
              scan_result->InitResult();
                                      cout<<"send to filter"<<endl;
          }
          scan_result->row_offset.push_back(scan_result->length);
          memcpy(scan_result->data+scan_result->length, row_data, row_size);
          scan_result->length += row_size;
          scan_result->row_count++;
          scan_result -> raw_row_count++;

      }

    } 

  }else{//index scan
    
  }
  
}

int getPrimaryKeyData(const char* ikey_data, char* dest, list<PrimaryKey> pk_list){
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
          cout << "new type!!-" << key_type << endl;
        }
      }
  }

  return pk_length;
}

void Scan::EnQueueData(Result scan_result, int scan_type){
    if(scan_type == Full_Scan_Filter){
      if(scan_result.length != 0){//scan->filter
        FilterQueue.push_work(scan_result);        
      }else{//scan->merge
        MergeQueue.push_work(scan_result);
      }

    }else if(scan_type == Full_Scan){//scan->merge
      // if(scan_result.filter_info.need_col_filtering){
      //   Column_Filtering(&scan_result, snippet_);
      // }
      MergeQueue.push_work(scan_result);

    }else if(scan_type == Index_Scan_Filter){//scan->filter
      if(scan_result.length != 0){
        FilterQueue.push_work(scan_result);
      }else{
        MergeQueue.push_work(scan_result);
      }
      
    }else{//Index_Scan, scan->merge
      // if(scan_result.filter_info.need_col_filtering){
      //   Column_Filtering(&scan_result, snippet_);
      // }
      MergeQueue.push_work(scan_result);
    }
}

char* hexstr_to_char(const char* hexstr, int* row_size){
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
string char_to_hexstr(const char *data, int len){
  std::string s(len * 2, ' ');
  for (int i = 0; i < len; ++i) {
    s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
    s[2 * i + 1] = hexmap[data[i] & 0x0F];
  }
  return s;
}


