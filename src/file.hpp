#ifndef FILE_HPP
#define	FILE_HPP

#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>


namespace snts {

class file {
public:
	file(const std::string& file_path, std::ios_base::openmode mode);
	~file();

	std::string getline();

	template <typename T>
	file& operator >>(T& dest) {
		this->check_for_input();
		this->file_stream >> dest;
		return *this;
	}

	template <typename T>
	file& operator <<(const T& src) {
		this->file_stream << src;
		return *this;
	}

	template <typename Function>
	Function for_each_line(Function f) {
		this->check_for_input();
		while (this->file_stream.good()) {
			f(this->getline());
		}
	}

private:
	void check_for_input() const;
	std::string file_path;
	std::fstream file_stream;
};


struct file_read_error : public std::runtime_error {
	file_read_error(const std::string& msg);
};

}

#endif	/* FILE_HPP */

