#include "db.h"
#include "Logger.h"
#include "leveldb/options.h"
#include <leveldb/db.h>
#include <string>

namespace Shadow {

Database::Database(std::string dbName)
	: dbName(dbName) {
	PRINT("Opened database %s", dbName.c_str());

	leveldb::Options options;
	options.create_if_missing = true;

	leveldb::Status s = leveldb::DB::Open(options, dbName, &db);

	if (!s.ok())
		ERROUT("%s", s.ToString().c_str());
}

Database::~Database() {
	delete db;
	PRINT("Database %s closed", dbName.c_str());
}

std::string Database::read(leveldb::Slice key) {
	std::string toReturn;
	db->Get(leveldb::ReadOptions(), key, &toReturn);
	return toReturn;
}

void Database::write(leveldb::Slice key, std::string value) {
	db->Put(leveldb::WriteOptions(), key, value);
}

void Database::remove(leveldb::Slice key) { db->Delete(leveldb::WriteOptions(), key); }

}