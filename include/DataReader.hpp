// TODO: MARKED FOR DELETION

#ifndef SHADOW_NATIVE_DATA_READER_HPP
#define SHADOW_NATIVE_DATA_READER_HPP

namespace Shadow::DataReader {

struct DataSource {
	enum Enum {
		Chunk,
		Filesystem,

		Count
	};
};

}

#endif /* SHADOW_NATIVE_DATA_READER_HPP */