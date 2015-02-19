/* Simple C program that connects to MySQL Database server*/
#include <mysql.h>

class CMySqlRow
{
	friend class CMySqlResult;

	MYSQL_ROW m_row;

	void Init(MYSQL_ROW row)
	{
		m_row = row;
	}

public:

	const char * GetString(uint column)
	{
		return m_row[column];
	}

	const int GetInt(uint column)
	{
		return atoi(GetString(column));
	}

	const float GetFloat(uint column)
	{
		return (float)atof(GetString(column));
	}
};


class CMySqlResult
{
	friend class CMySqlClient;

	MYSQL_RES * m_result;

	void Init(MYSQL_RES * res)
	{
		Free();
		m_result = res;
	}

	void Free()
	{
		if (NULL != m_result)
		{
			mysql_free_result(m_result);
			m_result = NULL;
		}
	}

public:

	CMySqlResult()
		: m_result(NULL)
	{
	}

	~CMySqlResult()
	{
		Free();
	}

	bool IsEmpty() const
	{
		return (NULL == m_result);
	}

	bool GetNextRow(CMySqlRow * outRow)
	{
		if (false == IsEmpty())
		{
			MYSQL_ROW row = mysql_fetch_row(m_result);
			if (NULL != row)
			{
				outRow->Init(row);
				return true;
			}
		}

		return false;
	}
};

class CMySqlClient
{
	MYSQL * m_connection;

public:

	CMySqlClient()
		: m_connection(NULL)
	{
		/*
		if (true == Connect(server, user, pwd, database))
		{
			CMySqlResult result;
			if (true == Query(&result, "show tables"))
			{
				// output table name
				printf("MySQL Tables in mysql database:\n");

				CMySqlRow row;
				while (true == result.GetNextRow(&row))
				{
					printf("%s \n", row.GetString(0));
				}	
			}
		}

		Close();
		*/
	}

	~CMySqlClient()
	{
		Close();
	}

	bool Query(CMySqlResult * outResult, const char * sqlQuery)
	{
		if (mysql_query(m_connection, sqlQuery))
		{
			fprintf(stderr, "%s\n", mysql_error(m_connection));
			return false;
		}
		
		if (NULL != outResult)
		{
			MYSQL_RES * res = mysql_use_result(m_connection);
			outResult->Init(res);
		}

		return true;
	}

	bool Connect(const char * server, const char * user, const char * pwd, const char * database)
	{
		if (NULL != m_connection)
		{
			FAIL("connection exist");
			return false;
		}

		m_connection = mysql_init(NULL);

		if (!mysql_real_connect(m_connection, server, user, pwd, database, 0, NULL, 0))
		{
			fprintf(stderr, "%s\n", mysql_error(m_connection));
			return false;
		}

		return true;
	}

	void Close()
	{
		if (NULL != m_connection)
		{
			mysql_close(m_connection);
			m_connection = NULL;
		}
	}
};