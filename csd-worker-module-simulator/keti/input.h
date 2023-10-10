#pragma once

#include "scan.h"
// #include "metric_check.h"

using namespace rapidjson;

extern WorkQueue<Snippet> ScanQueue;

void calculForReturnData(Snippet &snippet);//임시 추가 -> 나중엔 merge_manager에서 계산해서 넣어야함

class Input{
    public:
      inline const static std::string LOGTAG = "CSD Input";

      Input(){}
      void InputSnippet();
};

void Input::InputSnippet(){
        int server_fd;
        int client_fd;
        int opt = 1;
        struct sockaddr_in serv_addr; // 소켓주소
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(struct sockaddr_in);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
              perror("socket failed");
              exit(EXIT_FAILURE);
          }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
              perror("setsockopt");
              exit(EXIT_FAILURE);
          }
        cout<<"1"<<endl;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(5562); // port
        cout<<"2"<<endl;
        if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
          perror("bind");
          exit(EXIT_FAILURE);
        } // 소켓을 지정 주소와 포트에 바인딩
        cout<<"3"<<endl;
        if (listen(server_fd, 3) < 0){
          perror("listen");
          exit(EXIT_FAILURE);
        } // 리스닝
        cout<<"4"<<endl;
        while(1){
          if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) < 0){
            perror("accept");
                exit(EXIT_FAILURE);
          }
        cout<<"5"<<endl;
          std::string json;
          char buffer[BUFF_SIZE] = {0};
                  cout<<"6"<<endl;
          size_t length;
          int temp = read( client_fd , &length, sizeof(length));
          if(temp == 0){
            cout << "read error" << endl;
          }
        cout<<"7"<<endl;
          int numread;
          while(1) {
            cout<<"20"<<endl;
            if ((numread = read( client_fd , buffer, BUFF_SIZE - 1)) == -1) {
              cout << "read error" << endl;
              perror("read");
              exit(1);
            }
            cout<<"8"<<endl;
            cout<<length<<endl;
            cout<<numread<<endl;
            length -= numread;
              buffer[numread] = '\0';
            json += buffer;
            cout<<"9"<<endl;
              if (length == 0)
              break;
          }
            cout<<"10"<<endl;
            cout<<json<<endl;
            cout << "*******************Snippet JSON*****************" << endl;
            cout << json.c_str() << endl;
            cout << "************************************************" << endl;
          // //start
          // // send_Query_info
          // const char* message = "Query Start";
          // int sock = 0;
          // struct sockaddr_in serv_addr;

          // // Create socket file descriptor
          // if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
          //     std::cerr << "Socket creation error" << std::endl;
          //     // return -1;
          // }

          // serv_addr.sin_family = AF_INET;
          // serv_addr.sin_port = htons(8080);

          // // Convert IPv4 and IPv6 addresses from text to binary form
          // if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
          //     std::cerr << "Invalid address/ Address not supported" << std::endl;
          //     // return -1;
          // }

          // // Connect to the server
          // if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
          //     std::cerr << "Connection Failed" << std::endl;
          //     // return -1;
          // }

          // // Send message to the server
          // send(sock, message, strlen(message), 0);
          // std::cout << "Snippet start info sent" << std::endl;

          // close(sock);
            cout<<"11"<<endl;
          Snippet parsedSnippet(json.c_str());
                      cout<<"12"<<endl;
          calculForReturnData(parsedSnippet);
            cout<<"13"<<endl;
          //로그
          char msg[200];
          memset(msg, '\0', sizeof(msg));
          sprintf(msg,"Receive Snippet {ID : %d-%d} from Storage Engine Instance\n",parsedSnippet.query_id, parsedSnippet.work_id);
          KETILOG::INFOLOG(LOGTAG, msg);

          // printf("[CSD Input Interface] Receive Snippet {ID : %d-%d} from Storage Engine Instance\n",parsedSnippet.query_id, parsedSnippet.work_id);

          ScanQueue.push_work(parsedSnippet);
            cout<<"14"<<endl;
          close(client_fd);
        }

        close(server_fd);
      }

void calculForReturnData(Snippet &snippet){//임시작성
    unordered_map<string, int> col_type, col_offlen;
    for (int i = 0; i < snippet.table_col.size(); i++){
        col_type.insert(make_pair(snippet.table_col[i], snippet.table_datatype[i]));
        col_offlen.insert(make_pair(snippet.table_col[i], snippet.table_offlen[i]));
    }
    
    vector<int> return_datatype, return_offlen;
    for (int i = 0; i < snippet.column_projection.size(); i++){
        //tpc-h 쿼리 동작만 수행하도록 작성 => 수정필요
        if(snippet.column_projection[i].values[0] == "CASE"){
            return_datatype.push_back(3);
            return_offlen.push_back(4);
        }else if(snippet.column_projection[i].values[0] == "EXTRACT"){
            // return_datatype.push_back(14);
            // return_offlen.push_back(3);
            return_datatype.push_back(3);
            return_offlen.push_back(4);
        }else if(snippet.column_projection[i].values[0] == "SUBSTRING"){
            return_datatype.push_back(254);
            return_offlen.push_back(2);
        }else{
            if(snippet.column_projection[i].values.size() == 1){
                return_datatype.push_back(col_type[snippet.column_projection[i].values[0]]);
                return_offlen.push_back(col_offlen[snippet.column_projection[i].values[0]]);
            }else{
                int multiple_count = 0;
                for (int j = 0; j < snippet.column_projection[i].values.size(); j++){
                    if(snippet.column_projection[i].values[j] == "*"){
                      if(snippet.column_projection[i].values[j-1] == "ps_availqty"){//임시로 작성!!!!
                        multiple_count--;
                      }else{
                        multiple_count++;
                      }
                    }
                }
                if(multiple_count == 1){
                    return_datatype.push_back(246);
                    return_offlen.push_back(8);
                }else if(multiple_count == 2){
                    return_datatype.push_back(246);
                    return_offlen.push_back(9);
                }else{
                    return_datatype.push_back(col_type[snippet.column_projection[i].values[0]]);
                    return_offlen.push_back(col_offlen[snippet.column_projection[i].values[0]]);
                }
            }
        }
    }
    
    snippet.projection_datatype = return_datatype;
    snippet.projection_length = return_offlen;
}

    // int level_ = 0;
    // bool blocks_maybe_compressed_ = true;
    // bool blocks_definitely_zstd_compressed_ = false;
    // const bool immortal_table_ = false;
    // Slice cf_name_for_tracing_ = nullptr;
    // uint64_t sst_number_for_tracing_ = 0;
    // shared_ptr<Cache> to_block_cache_ = nullptr;//to_ = table_options
    // uint32_t to_read_amp_bytes_per_bit = 0;
    // //std::shared_ptr<const FilterPolicy> to_filter_policy = nullptr;
    // //std::shared_ptr<Cache> to_block_cache_compressed = nullptr;
    // bool to_cache_index_and_filter_blocks_ = false;
    // //ioptions
    // uint32_t footer_format_version_ = 5;//footer
    // int footer_checksum_type_ = 1;
    // uint8_t footer_block_trailer_size_ = 5;
    
    // //std::string dev_name = "/usr/local/rocksdb/000051.sst";
    // /*block info*/

    // std::string dev_name = "/dev/sda";
    // // const uint64_t handle_offset = 43673280512;
    // // const uint64_t block_size = 3995; //블록 사이즈 틀리지 않게!!


    // 		//-------test----------------------
// 	char json[8000];
// 	int i=0;
// 	memset(json,0,sizeof(json));
// 	int json_fd = open("test.json",O_RDONLY);
// 	while(1){
// 		int res = read(json_fd,&json[i++],1);
// 		if(res == 0){
// 			break;
// 		}
// 	}
// 	close(json_fd);
// 	// cout << "*******************Snippet JSON*****************" << endl;
//   	// cout << json << endl;
//   	// cout << "************************************************" << endl;

// 	Snippet parsedSnippet(json);
// 	//로그
// 	// printf("---------------------------------------------:: STEP3 ::---------------------------------------------\n");
// 	printf("[CSD Input Interface] Recieve Snippet {ID : %d-%d} from Storage Engine Instance\n",parsedSnippet.query_id,parsedSnippet.work_id);
// 	EnQueueScan(parsedSnippet);
// 	//-----------------------------
	
// }

// void Input::EnQueueScan(Snippet parsedSnippet_){
// 	ScanQueue.push_work(parsedSnippet_);
