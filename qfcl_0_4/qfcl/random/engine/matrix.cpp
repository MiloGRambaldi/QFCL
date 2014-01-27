#include <boost/filesystem.hpp>

#include "matrix.hpp"

namespace qfcl {

namespace random {

template<>
NTL::mat_GF2 identity<NTL::mat_GF2>(const NTL::mat_GF2 & M)
{
	return NTL::ident_mat_GF2( M.NumRows() );
}

// write
void Matrix<2>::write(const std::string & filename) const throw(std::runtime_error)
{
	using namespace std;

	ofstream ofs( filename.c_str(), ios::binary );

	if (ofs)
	{
		// sizeof element must be 1
		ostreambuf_iterator<char> iter( ofs.rdbuf() );

		// write the shape first
		long rows = NumRows();
		long cols = NumCols();
		qfcl::io::write_char_n( &rows, sizeof(rows), iter );
		qfcl::io::write_char_n( &cols, sizeof(cols), iter );
	
		for (long i = 0; i < rows; ++i)
		{
			qfcl::io::write_char_n( (*this)[i].rep.rep, (*this)[i].rep.length() * sizeof(_ntl_ulong), iter );
		}

		ofs.close();
	}
	else
	{
		throw std::runtime_error("Error in matrix_GF2<T>::write: Unable to open file for writing.");
	}
}

// read, return value indicates whether matrix file exists
bool Matrix<2>::read(const std::string & filename) throw(std::runtime_error)
{
	using namespace std;

	// check if file exists
	boost::filesystem::path matrix_file(filename);
	if (!boost::filesystem::exists(matrix_file)) 
		return false;

	ifstream ifs( filename.c_str(), ios::binary );

	//auto state = ifs.rdstate();

	if (ifs)
	{
		// sizeof element must be 1
		istreambuf_iterator<char> iter( ifs.rdbuf() );

		// read the shape first
		long rows;
		long cols;
		qfcl::io::read_char_n( &rows, sizeof(rows), iter );
		qfcl::io::read_char_n( &cols, sizeof(cols), iter );

		SetDims(rows, cols);
	
		for (long i = 0; i < rows; ++i)
		{
			qfcl::io::read_char_n( (*this)[i].rep.rep, (*this)[i].rep.length() * sizeof(_ntl_ulong), iter );
		}

		ifs.close();
	}
	else
	{
		throw std::runtime_error("Error in matrix_GF2<T>::read: Unable to open file for reading.");
	}

	return true;
}

template<>
Matrix<2> identity< Matrix<2> >(const Matrix<2> & M)
{
	return NTL::ident_mat_GF2( M.NumRows() );
}

}	// namespace random

}	// namespace qfcl
