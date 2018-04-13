/** 
 * Copyright (c) 2016 SQLines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// SqlOdbcApi ODBC API

#ifndef sqlines_sqlodbcapi_h
#define sqlines_sqlodbcapi_h

#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h> 
#include "sqlapibase.h"
#include "sqldb.h"

class SqlOdbcApi : public SqlApiBase
{
	// Database type
	short _db_type;

	// Environment and connection handles
	SQLHENV _henv;
	SQLHDBC _hdbc;
	SQLHANDLE _hstmt_cursor;

	// Connection information
	std::string _user;
	std::string _pwd;
	std::string _server;
	std::string _instance;
	std::string _port;
	std::string _db;
	std::string _driver;
	std::string _conn;

	// Attribute to store last number of fetched rows (SQL_ATTR_ROWS_FETCHED_PTR)
	int _cursor_fetched;

	// SQLGetData feature supported (SQL_GETDATA_EXTENSIONS attribute)
	SQLUINTEGER _sqlGetData_features;

public:
	SqlOdbcApi();
	~SqlOdbcApi();

	// Initialize API
	virtual int Init();

	// Set additional information about the driver type
	virtual void SetDriverType(const char *info);

	// Set the connection string in the API object
	virtual void SetConnectionString(const char *conn);

	// Connect to the database
	virtual int Connect(size_t *time_spent);

	// Get row count for the specified object
	virtual int GetRowCount(const char *object, long *count, size_t *time_spent);

	// Execute the statement and get scalar result
	virtual int ExecuteScalar(const char *query, long *result, size_t *time_spent);

	// Execute the statement
	virtual int ExecuteNonQuery(const char *query, size_t *time_spent);

	// Open cursor and allocate buffers
	virtual int OpenCursor(const char *query, long buffer_rows, long buffer_memory, long *col_count, long *allocated_array_rows, 
		long *rows_fetched, SqlCol **cols, size_t *time_spent, bool catalog_query = false,
        std::list<SqlDataTypeMap> *dtmap = NULL);

	// Fetch next portion of data to allocate buffers
	virtual int Fetch(long *rows_fetched, size_t *time_spent);

	// Close the cursor and deallocate buffers
	virtual int CloseCursor();

	// Initialize the bulk copy from one database into another
	virtual int InitBulkTransfer(const char *table, long col_count, long allocated_array_rows, SqlCol *s_cols, SqlCol **t_cols);

	// Transfer rows between databases
	virtual int TransferRows(SqlCol *s_cols, long rows_fetched, long *rows_written, size_t *bytes_written,
		size_t *time_spent);

	// Specifies whether API allows to parallel reading from this API and write to another API
	virtual bool CanParallelReadWrite();

	// Complete bulk transfer
	virtual int CloseBulkTransfer();

	// Drop the table
	virtual int DropTable(const char* table, size_t *time_spent, std::string &drop_stmt);
	// Remove foreign key constraints referencing to the parent table
	virtual int DropReferences(const char* table, size_t *time_spent);

	// Get the length of LOB column in the open cursor
	virtual int GetLobLength(long row, long column, long *length);
	// Get LOB content
	virtual int GetLobContent(long row, long column, void *data, long length, long *len_ind);
	// Get partial LOB content
	virtual int GetLobPart(long row, long column, void *data, long length, long *len_ind);
	
	// Get the list of available tables
	virtual int GetAvailableTables(std::string &select, std::string &exclude, std::list<std::string> &tables);

	// Read schema information
	virtual int ReadSchema(const char *select, const char *exclude, bool read_cns = true, bool read_idx = true);

	// Read specific catalog information
	virtual int ReadConstraintTable(const char *schema, const char *constraint, std::string &table);
	virtual int ReadConstraintColumns(const char *schema, const char *table, const char *constraint, std::string &cols);

	// Build a condition to select objects from the catalog
	virtual void GetCatalogSelectionCriteria(std::string &selection_template, std::string &output);

	// Get database subtype
	virtual short GetType() { return SQLDATA_ODBC; }
	virtual short GetSubType() { return _db_type; }

private:
	// Free connection and environment handles 
	void Deallocate();

	// Set the connection string to connect to Oracle
	void SetOracleConnectionString(const char *conn);

	// Write LOB data using BCP API
	int WriteLob(SqlCol *s_cols, int row, int *lob_bytes);

	// Set error code and message for the last API call
	void SetError(SQLSMALLINT handle_type, SQLHANDLE handle);
};

#endif // sqlines_sqlodbcapi_h
