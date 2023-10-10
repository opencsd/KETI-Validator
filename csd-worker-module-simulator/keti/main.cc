// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory)
#include <thread>

#include "input.h"

using namespace std;

// ---------------현재 사용하는 인자(compression, cache X)-----------
// bool blocks_maybe_compressed = false;
// bool blocks_definitely_zstd_compressed = false;
// uint32_t read_amp_bytes_per_bit = 0;
// const bool immortal_table = false;
// std::string dev_name = "/dev/sda";
// -----------------------------------------------------------------

WorkQueue<Snippet> ScanQueue;
WorkQueue<Result> FilterQueue;
WorkQueue<Result> MergeQueue;
WorkQueue<MergeResult> ReturnQueue;

int main(int argc, char** argv) {
    if (argc >= 2) {
        KETILOG::SetLogLevel(stoi(argv[1]));
    }else{
        KETILOG::SetDefaultLogLevel();
    }
        TableManager CSDTableManager;
    CSDTableManager.InitCSDTableManager();

    thread InputInterface = thread(&Input::InputSnippet, Input());
    // Scan scan = Scan(CSDTableManager);
    // scan.Scanning();
    thread ScanLayer = thread(&Scan::Scanning, Scan(CSDTableManager));
    thread FilterLayer1 = thread(&Filter::Filtering, Filter());
    // thread FilterLayer2 = thread(&Filter::Filtering, Filter());
    // thread FilterLayer3 = thread(&Filter::Filtering, Filter());
    thread MergeLayer = thread(&MergeManager::Merging, MergeManager());
    thread ReturnInterface = thread(&Return::ReturnResult, Return());

    InputInterface.join();
    ScanLayer.join();
    FilterLayer1.join();
    // FilterLayer2.join();
    // FilterLayer3.join();
    MergeLayer.join();
    ReturnInterface.join();
    
    return 0;
}
