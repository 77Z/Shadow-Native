#include "shadow_fs.h"

// Handles json config file reading and writing,
// and general reading and writing of files,
// and basic LevelDB control

#include <leveldb/db.h>
#include <string>
#include "Logger.h"

#include "lz4.h"

leveldb::DB* Shadow::fs::openDB(std::string dbname) {
	leveldb::DB* db;

	leveldb::Options options;
	options.create_if_missing = true;

	leveldb::Status s = leveldb::DB::Open(options, dbname, &db);

	if (!s.ok())
		ERROUT(s.ToString());

	return db;
}

void Shadow::fs::closeDB(leveldb::DB* db) {
	delete db;
}

void Shadow::fs::writeDB(leveldb::DB* db, leveldb::Slice key, std::string value) {
	db->Put(leveldb::WriteOptions(), key, value);
}

