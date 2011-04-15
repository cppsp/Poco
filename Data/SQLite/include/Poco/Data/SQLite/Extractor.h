//
// Extractor.h
//
// $Id: //poco/1.4/Data/SQLite/include/Poco/Data/SQLite/Extractor.h#1 $
//
// Library: Data/SQLite
// Package: SQLite
// Module:  Extractor
//
// Definition of the Extractor class.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef DataConnectors_SQLite_Extractor_INCLUDED
#define DataConnectors_SQLite_Extractor_INCLUDED


#include "Poco/Data/SQLite/SQLite.h"
#include "Poco/Data/AbstractExtractor.h"
#include "Poco/Any.h"


struct sqlite3_stmt;


namespace Poco {
namespace Data {
namespace SQLite {


class SQLite_API Extractor: public Poco::Data::AbstractExtractor
	/// Extracts and converts data values form the result row returned by SQLite.
	/// If NULL is received, the incoming val value is not changed and false is returned
{
public:
	Extractor(sqlite3_stmt* pStmt);
		/// Creates the Extractor.

	~Extractor();
		/// Destroys the Extractor.

	bool extract(std::size_t pos, Poco::Int8& val);
		/// Extracts an Int8.

	bool extract(std::size_t pos, Poco::UInt8& val);
		/// Extracts an UInt8.

	bool extract(std::size_t pos, Poco::Int16& val);
		/// Extracts an Int16.

	bool extract(std::size_t pos, Poco::UInt16& val);
		/// Extracts an UInt16.

	bool extract(std::size_t pos, Poco::Int32& val);
		/// Extracts an Int32.

	bool extract(std::size_t pos, Poco::UInt32& val);
		/// Extracts an UInt32.

	bool extract(std::size_t pos, Poco::Int64& val);
		/// Extracts an Int64.

	bool extract(std::size_t pos, Poco::UInt64& val);
		/// Extracts an UInt64.

	bool extract(std::size_t pos, bool& val);
		/// Extracts a boolean.

	bool extract(std::size_t pos, float& val);
		/// Extracts a float.

	bool extract(std::size_t pos, double& val);
		/// Extracts a double.

	bool extract(std::size_t pos, char& val);
		/// Extracts a single character.

	bool extract(std::size_t pos, std::string& val);
		/// Extracts a string.

	bool extract(std::size_t pos, Poco::Data::BLOB& val);
		/// Extracts a BLOB.

	bool extract(std::size_t pos, Poco::Any& val);
		/// Extracts an Any.

private:
	bool isNull(std::size_t pos);
		/// Returns true if a non null value can be extracted

	sqlite3_stmt* _pStmt;
};


} } } // namespace Poco::Data::SQLite


#endif // DataConnectors_SQLite_Extractor_INCLUDED
