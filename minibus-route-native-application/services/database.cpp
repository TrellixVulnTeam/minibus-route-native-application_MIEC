#include "database.h"
#include "logger.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <sharedsettings.h>
#include <QtSql/qsqldatabase>
#include <encryption/encrypto.h>


/*! 
 * \brief - Database constructor
 *
 * \param - QObject *parent (Input)
 * 
 */
Database::Database( QObject *parent ) : QObject( parent )
{

}

Database::Database(QString driver, QObject *parent): QObject( parent )
{
    setDriver(driver);
}


/*! 
 * \brief 	- Establishes a connection to database
 *
 * \return bool - true if successfully connects to database
 *              - false otherwise
 */
bool Database::connOpen()
{

    if( this->getDriver() == "QSQLITE"  )
    {
        // decrypt the database
        if(SharedSettings::instance()->getLocalDbPath() != "")
        {
            Encrypto decrypt_module;
            QByteArray db_data;
            db_data = UtilityFunction::instance()->readFile(SharedSettings::instance()->getLocalDbPath());
            QByteArray dec_data = decrypt_module.decryptAES(UtilityFunction::instance()->getPassphrase().toLatin1(), db_data);
            UtilityFunction::instance()->writeFile(SharedSettings::instance()->getLocalDbPath(), dec_data);
            qDebug() << "DB Decrypted: " << SharedSettings::instance()->getLocalDbPath();

              return initSQLite();
        }

    }
    else if (this->getDriver() == "QPSQL")
    {
        if(SharedSettings::instance()->getRemoteDb() != "")
        {
            return initPostGres();
        }
    }
    return false;

}

/*!
 * \brief  - Closes a connection to database
 * 
 * \return void
 */
void Database::connClosed()
{
    if(SharedSettings::instance()->getRemoteDb() != 0)
    {
        database.close();
        database.removeDatabase(QSqlDatabase::defaultConnection);

        if(!isPostGres())
        {
            // is SQLITE = true
            // encryption of database
            Encrypto Encrypt_module;
            QByteArray db_data = UtilityFunction::instance()->readFile(SharedSettings::instance()->getLocalDbPath());
            QByteArray en_data = Encrypt_module.encryptAES(UtilityFunction::instance()->getPassphrase().toLatin1(), db_data, false);
            UtilityFunction::instance()->writeFile(SharedSettings::instance()->getLocalDbPath(), en_data);
            qDebug() << "DB Encrypted: " << SharedSettings::instance()->getLocalDbPath();
        }
        else
            qDebug() << "Postgres closed";
    }
}

/*!
 * \brief    	  - Converts vector into merged string tokens
 *                   seperated by the comma delimeter
 *
 * \param    	  - QVector<QString> value (Input) 
 * 
 * \return bool   - QString strBuilder
 */
QString Database::toString( QVector<QString> &value )
{
   QString strBuilder = "";

   for(int i = 0 ; i<value.size() ; i++)
   {
       if( value.size() == 1)
       {
           strBuilder += value[i];
           return strBuilder;
       }

       else if ( value.size() > 1 && i < (value.size() - 1) )
       {
           strBuilder += value[i];
           strBuilder += ", ";
       }
       else
       {
           strBuilder += value[i];
           return strBuilder;
       }
   }
}

/*! 
 * \brief 	 - Establishes a connection to the SQLite database
 *
 * \return bool  - true if successfully connects to database
 *               - false otherwise
 */
bool Database::initSQLite()
{
    database = QSqlDatabase::addDatabase(getDriver());
    database.setDatabaseName(SharedSettings::instance()->getLocalDbPath());

    if(!database.open())
    {
        qDebug()<<"\t #### SQLite database: failed to open the database";

        return  false;
    }
    else
    {
       qDebug()<<"\t #### SQLite database: connected..... ";
       return true;
    }

}


/*!
 * \brief	- Initializes the Postgresql database
 *	           with all required connection strings
 *
 * \return bool - true if successfully initializes database object
 * 	        - false otherwise
 */
bool Database::initPostGres()
{
    qDebug() << "getDriver(): " << getDriver();
    qDebug() << "SharedSettings::instance()->getRemoteDb(): " << SharedSettings::instance()->getRemoteDb();
    qDebug() << "SharedSettings::instance()->getRemoteDbPort(): " << SharedSettings::instance()->getRemoteDbPort();
    qDebug() << "SharedSettings::instance()->getRemoteDbName(): " << SharedSettings::instance()->getRemoteDbName();
    qDebug() << "SharedSettings::instance()->getRemoteDbUsername(): " << SharedSettings::instance()->getRemoteDbUsername();
    qDebug() << "SharedSettings::instance()->getRemoteDbPassword(): " << SharedSettings::instance()->getRemoteDbPassword();

    database = QSqlDatabase::addDatabase(getDriver());
    database.setHostName(SharedSettings::instance()->getRemoteDb());
    database.setPort(SharedSettings::instance()->getRemoteDbPort());
    database.setDatabaseName(SharedSettings::instance()->getRemoteDbName());
    database.setUserName(SharedSettings::instance()->getRemoteDbUsername());
    database.setPassword(SharedSettings::instance()->getRemoteDbPassword());

    if(!database.open())
    {
        qDebug() << "Database::initPostGres() - Failed to open Postgres DB";
        return false;
    }
    else
    {
        qDebug() << "Database: Postgres Connected..... ";
        return true;
    }
}

/*! 
 * \brief 		- Database Object accessor
 *
 * \return QSqlDatabase -  database object
 *
 */
QSqlDatabase Database::getDatabase() const
{
    return database;
}

/*! 
 * \brief - Database Object mutator
 *
 * \param - QSqlDatabase object (Input)
 *
 * \return void
 * 
 */
void Database::setDatabase( const QSqlDatabase &value )
{
    database = value;
}

/*! 
 * \brief 		- Database host name accessor
 *
 * \return QString 	- Host name
 * 
 */
QString Database::getHost() const
{
    return host;
}

/*! 
 * \brief - Database host name mutator
 *
 * \param - QString host name (Input)
 *
 * \return void
 * 
 */
void Database::setHost( const QString &value )
{
    host = value;
}

/*!
 * \brief 	     - Database driver name accessor
 *
 * \return QString   - Driver name
 * 
 */
QString Database::getDriver() const
{
    return driver;
}

/*! 
 * \brief - Database driver name mutator
 *
 * \param - QString driver name (Input)
 *
 * \return void
 * 
 */
void Database::setDriver( const QString &value )
{
    driver = value;
}

/*! 
 * \brief	- Checks if database driver is Postgresql
 *
 * \return bool	- true if driver is Postgresql
 *         	- false if otherwise
 */
bool Database::isPostGres()
{
    if(getDriver() == "QPSQL")
        return true;
    else
        return false;
}

/*!
 * \brief 	    - Database name accesor
 *
 * \return QString  - Database name
 * 
 */
QString Database::getDbname() const
{
    return dbname;
}

/*! 
 * \brief - Database name mutator
 *
 * \param - QString database name (Input)
 * 
 * \return void
 * 
 */
void Database::setDbname( const QString &value )
{
    dbname = value;
}

/*! 
 * \brief 	    - Database username accesor
 *
 * \return QString  - Database name
 * 
 */
QString Database::getUsername() const
{
    return username;
}

/*! 
 * \brief - Database username mutator
 *
 * \param - QString username (Input)
 *
 * \return void
 * 
 */
void Database::setUsername( const QString &value )
{
    username = value;
}

/*! 
 * \brief 	    - Database password accesor
 *
 * \return QString  - Password
 * 
 */
QString Database::getPassword() const
{
    return password;
}

/*! 
 * \brief - Database password mutator
 *
 * \param - QString password (Input)
 *
 * \return void
 * 
 */
void Database::setPassword( const QString &value )
{
    password = value;
}

/*! 
 * \brief 	- Database port number accesor
 *
 * \return int  - Port number
 * 
 */
int Database::getPort() const
{
    return port;
}

/*! 
 * \brief - Database port number mutator
 *
 * \param - int port number (Input)
 *
 * \return void
 * 
 */
void Database::setPort( int value )
{
    port = value;
}


/*!
 * \brief Database::selectWithCondition
 * \param table
 * \param select_columns
 * \param column_list
 * \param value_list
 * \param columnCondition
 * \param conditionValue
 * \param result
 * \return
 */
bool Database::selectWithCondition(  QString table,
                        QVector<QString> &select_columns,\
                        QVector<QString> &column_list,\
                        QVector<QString> &value_list,\
                        QString columnCondition,\
                        QString conditionValue,\
                        QVector<QSqlRecord> &result)
{
    // clear the results
    result.clear();

    // link database to query
    QSqlQuery query(getDatabase());

    // check if query is executed
    bool isQueryExecuted;

    QString strSelectList = toString(select_columns);

    QString strQuery = "";


    strQuery = "SELECT " + strSelectList + " FROM " + table +
               " WHERE " + columnCondition + " = '" + conditionValue + "'";

    qDebug() << "strQuery: " << strQuery;

    // string query builder
    for( int i = 0 ; i < column_list.size() ; i++)
    {

        if ( column_list.size() == 1 || i == (value_list.size() - 1) )
        {
            strQuery += ( column_list[i] + "=:" + column_list[i] );

        }
        else
            strQuery += (column_list[i] + "=:" + column_list[i] + " AND ");
    }


    // prepare query
     query.prepare(strQuery);

    // bind parameters
    for (int i = 0; i < column_list.size() ; i++)
    {
        query.bindValue(":"+column_list[i], value_list[i]);
    }

    qDebug() << "Select Query : " << strQuery;
    // execute query
    isQueryExecuted = query.exec();

    if( !isQueryExecuted )
    {
        qDebug() << "database::select() query.exec() Error: " << query.lastError().text();
        qDebug() << "database error code: " << query.lastError().number();

        return isQueryExecuted;
    }

    // save records to the results vector
    while(query.next())
    {
        result.push_back(query.record());
    }


    return isQueryExecuted;
}

/*!
 * \brief Select function                    - Retrieves data from database table
 *
 * \param QString table                      - specifies name of the db table (Input)
 * \param QVector<QString> select_columns    - specifies fields to fetch data from (Input) 
 * \param QVector<QString> column_list       - specifies which columns to filter by (Input)
 * \param QVector<QString> value_list        - specifies the values for the columns (Input)
 * \param QVector<QSqlRecord> result         - a vector to store results (Output)
 *
 * \return bool           		     - true if query executed successfully
 *                          		     - false if query execution failed
 */
bool Database::select(  QString table,
                        QVector<QString> &select_columns,
                        QVector<QString> &column_list,
                        QVector<QString> &value_list,
                        QVector<QSqlRecord> &result
                      )
{
    // clear the results
    result.clear();

    // link database to query
    QSqlQuery query(getDatabase());

    // check if query is executed
    bool isQueryExecuted;

    // declare a record
    QSqlRecord rec;

    QString strSelectList = toString(select_columns);
    //QString strColumnList = toString(column_list) ;

    QString strQuery = "";

    if( !(column_list.isEmpty() && value_list.isEmpty()) )
    {
        qDebug() << "$$$$$$$$-> ";
        strQuery = "SELECT " + strSelectList + " FROM " + table +
                   " WHERE ";

        // string query builder
        for( int i = 0 ; i < column_list.size() ; i++)
        {

            if ( column_list.size() == 1 || i == (value_list.size() - 1) )
            {
                strQuery += ( column_list[i] + "=:" + column_list[i] );

            }
            else
                strQuery += (column_list[i] + "=:" + column_list[i] + " AND ");
        }


        // prepare query
        query.prepare(strQuery);

        // bind parameters
        for (int i = 0; i < column_list.size() ; i++)
        {
            qDebug() << " " << value_list[i].toStdString().c_str();
            query.bindValue(":"+column_list[i], value_list[i].toStdString().c_str());
        }

    }
    else
    {
        qDebug() << "$$$$$$$$-> 2";

        strQuery = " SELECT " + strSelectList + " FROM " + table;

        // prepare query
        query.prepare(strQuery);

    }

    qDebug() << "Select Query : " << strQuery;
    // execute query
    isQueryExecuted = query.exec();

    if( !isQueryExecuted )
    {
        qDebug() << "database::select() query.exec() Error: " << query.lastError().text();
        qDebug() << "database error code: " << query.lastError().number();
        return isQueryExecuted;
    }

    // save records to the results vector
    while(query.next())
    {
        result.push_back(query.record());
    }


    return isQueryExecuted;
}

/*!
 * \brief test Insert function      - Saves template data to database table
 *
 * \param Qstring table             - specifies name of the db table (Input)
 * \param QByteArray value          - Values to store in columns (Input)
 *
 * \return bool		            - true if query executed successfully
 *                                  - false if query execution failed
 */
bool Database::testInsertinToDb( QString table,
                                 QByteArray value)
{
  QSqlQuery query;

  // check if query is executed
  bool isQueryExecuted;

  QString strQuery = "INSERT INTO " + table + " ( template_1) VALUES( :template_1 )";

  // prepare statement
  query.prepare(strQuery);

  // bind parameters

  query.bindValue(":template_1" , value);

  isQueryExecuted = query.exec();

  if( !isQueryExecuted )
  {
      qDebug() << "Database::testInsertinToDb() - database::insert() query.exec() Error: " << query.lastError().text();
      qDebug() << "Database::testInsertinToDb() - database error code: " << query.lastError().number();

      return isQueryExecuted;
  }
  else
    qDebug() << "SUCCESS !!!!!!!!!!! ;) - yeah finally";

}

/*!
 * \brief Insert function           - Saves template data to database table
 *
 * \param Qstring table             - specifies name of the db table (Input)
 * \param QString employeeID        - specifies columns for storing (Input)
 * \param QByteArray templateBio    - specifies the values to store in columns (Input)
 *
 * \return bool		            - true if query executed successfully
 *                                  - false if query execution failed
 */
bool Database::insertTemplate( QString table,
                               QString employeeID,
                               QByteArray templateBio)
{

    QSqlQuery query;

    // check if query is executed
    bool isQueryExecuted;

    QString strQuery = "INSERT INTO " + table + " ( employee_id, template_one) VALUES( :employeeID, :template_one )";

    qDebug() << "HERE String Query : " << strQuery;

    query.prepare(strQuery);

    query.bindValue(":employeeID", employeeID);

    if(isPostGres())
        query.bindValue(":template_one" , templateBio.toBase64());
    else
        query.bindValue(":template_one" , templateBio);

    isQueryExecuted = query.exec();

    if( !isQueryExecuted )
    {
        qDebug() << "Database::insertTemplate() -  query.exec() Error: " << query.lastError().text();
        qDebug() << "Database::insertTemplate() -  database error code: " << query.lastError().number();

        return isQueryExecuted;
    }

    return isQueryExecuted;

}

/*!
 * \brief Retrieves template data from the database 
 *
 * \param Qstring ColumnName        - Template Column name in DB table (Input)
 * \param QString tableName         - Table name where template data resides (Input)
 * \param QString condition         - filtering key (Input)
 * \param QString conditionValue    - filtering value (Input)
 * \param QByteArray data	        - Template data (Input)
 *
 * \return bool		            - true if query executed successfully
 *                                  - false if query execution failed
 */
bool Database::retrieveData( QString columnName,\
                             QString tableName,\
                             QString condition,\
                             QString conditionValue,\
                             QList<QByteArray> &data)
{
  // initiliase
  QSqlQuery query;
  QModelIndex index;
  QSqlQueryModel model;

  // set query
  QString query_string = ("SELECT " + columnName + " FROM [" + tableName + "] WHERE [" + condition + "] ="+ conditionValue + ";");
  query.prepare(query_string);

  qDebug() << "query_string: " << query_string;
  // execute query

  bool isQueryExecuted = query.exec();

  if( isQueryExecuted )
  {
      // - Set model
      model.setQuery(query);

      // GET ALL PRINTS FROM DATABASE (TOTAL 3-PRINTS)
      int three_impressions = 3;
      for(int current_fingerprint = 0; current_fingerprint < three_impressions; current_fingerprint++)
      {
          // - set index
          index = model.index(current_fingerprint,0);

          // - get data
          data.append(QByteArray::fromBase64(index.data().toByteArray()));
      }
  }
  else
  {
    qDebug() << "Failed to receive data from " << tableName;
  }

  return isQueryExecuted;
}


bool Database::insertImageIntoDb(QString tableName,\
                                 QString columnName,\
                                 QString columnCondition,\
                                 QString columnnConditionValue,\
                                 QByteArray image)
{
    QSqlQuery query;
    query.prepare("UPDATE " + tableName +
                  " set " + columnName +
                  " = :" + columnName +
                  " where "+
                  columnCondition +" = '"+columnnConditionValue+"'");

    if(isPostGres())
        query.bindValue((":"+columnName), image.toBase64());
    else
        query.bindValue((":"+columnName), image);

    if(!(query.exec()))
    {
        qDebug()<< " Error inserting into: " <<  tableName << ", " << query.lastError();
        return false;
    }

    return true;
}

/*!
 * \brief Insert function                    - Insert data to database table
 *
 * \param QString table                      - specifies name of the db table (Input)
 * \param QVector<QString> column_list       - specifies which columns to filter by (Input)
 * \param QVector<QString> value_list        - specifies the values for the columns (Input)
 *
 * \return bool            		     - true if query executed successfully
 *                          		     - false if query execution failed
 */
bool Database::insert( QString table,
                       QVector<QString> &column_list,
                       QVector<QString> &value_list
                     )
{
    QSqlQuery query;

    // check if query is executed
    bool isQueryExecuted = false;

    QString strQuery = "INSERT INTO " + table + " (";

    // specify columns
    for (int i = 0; i < column_list.size() ; i++)
    {
        if ( column_list.size() == 1 )
        {
            strQuery += column_list[i] + ") ";
        }
        else if ( column_list.size() > 1 && i< column_list.size() - 1 )
        {
            strQuery += column_list[i] + ", ";
        }
        else
        {
            strQuery += column_list[i] + ") ";
        }
    }

    //specify values
    strQuery += "VALUES(";
    for (int i = 0; i < column_list.size() ; i++)
    {
        if ( column_list.size() == 1 )
        {
            strQuery += ":" + column_list[i] + ") ";


        }
        else if ( column_list.size() > 1 && i < column_list.size() - 1 )
        {
            strQuery += ":" + column_list[i] + ", ";
        }
        else
        {
            strQuery += ":" + column_list[i] + ") ";
        }
    }

    qDebug() << "---->>> Sql query : " << strQuery;

    // prepare statement
    query.prepare(strQuery);

    // bind parameters
    if( !(column_list.isEmpty() && value_list.isEmpty()) )
    {
        for (int i = 0; i < column_list.size() ; i++)
        {
            query.bindValue(":"+column_list[i], value_list[i]);
        }
    }

    isQueryExecuted = query.exec();

    if( !isQueryExecuted )
    {
        qDebug() << "Database::insert() - query.exec() Error: " << query.lastError().text();
        qDebug() << "Database::insert() - database error code: " << query.lastError().number();

        return isQueryExecuted;
    }

    return isQueryExecuted;
}

/*!
 * \brief Remove function                    - Deletes data from data table filtered by column values
 *
 * \param QString table                      - specifies name of the db table (Input)
 * \param QVector<QString> column_list       - specifies which columns to filter by (Input)
 * \param QVector<QString> value_list        - specifies the values for the columns (Input)
 *
 * \return bool         		     - true if query executed successfully
 *                          		     - false if query execution failed
 */
bool Database:: remove( QString table, 
                        QVector<QString> &column_list, 
                        QVector<QString> &value_list)
{
    QSqlQuery query( getDatabase());
    QString strQuery = "";

    bool isQueryExecuted;

    //check records in the table
    if( !(column_list.isEmpty() && value_list.isEmpty()) )
    {
        strQuery = "DELETE FROM " + table +  " WHERE " ;

        for(int i=0; i < column_list.size(); i++)
        {

            if ( column_list.size() == 1 || i == (value_list.size() - 1) )
                strQuery += ( column_list[i] + "=:" + column_list[i] );
            else
                strQuery += (column_list[i] + "=:" + column_list[i] + " AND ");
        }

        query.prepare(strQuery);

        for (int i = 0; i < column_list.size() ; i++)
        {
            query.bindValue(":"+column_list[i], value_list[i]);
        }

    }
    else
    {
        strQuery = "Delete FROM " + table;
        query.prepare(strQuery);
    }

    isQueryExecuted = query.exec();

    if( !isQueryExecuted )
    {
        qDebug() << "database::delete() query.exec() Error: " << query.lastError().text();
        qDebug() << "database error code: " << query.lastError().number();

        return isQueryExecuted;
    }

    return isQueryExecuted;
}


/*!
 * \brief Update function                    	 - Updates database table
 *
 * \param QString table                          - Specifies name of the db table (Input)
 * \param QVector<QString> select_columns        - Specifies fields to fetch data from (Input) 
 * \param QVector<QString> column_list           - Specifies which columns to update (Input)
 * \param QVector<QString> value_list            - Specifies which columns with specific 
 * 						   values to update (Input)
 * \param QVector<QString> column_list_condition - Specifies columns to filter by (Input)
 * \param QVector<QString> value_list_condition  - Specifies values to filter by (Input)
 *
 * \return bool            		     	 - true if query executed successfully
 *                          		         - false if query execution failed
 */
bool Database::update(QString table, QVector<QString>&column_list,
                      QVector<QString> &value_list,
                      QVector<QString> &column_list_condition,
                      QVector<QString> &value_list_condition)
{

    qDebug() << "#1";
    QString strQuery = "";
    unsigned int empty = 0;
    bool isQueryExecuted;
    QSqlQuery query(getDatabase());

    if( !(column_list.isEmpty() && value_list.isEmpty()) )
    {
        qDebug() << "#2";

      strQuery = "UPDATE "+ table + " SET " ;

      for(int i=0; i< column_list.size(); i++)
      {

          if ( column_list.size() == 1 )
          {
              strQuery += ( column_list[i] + "=:" + column_list[i] );
          }

          else if ( column_list.size() > 1 && i< column_list.size() - 1 )
          {
              strQuery +=  ( column_list[i] + "=:" + column_list[i] ) + " , ";
          }

          else
              strQuery += (column_list[i] + "=:" + column_list[i]);


      }

      qDebug() << "#3";


      if ( column_list_condition.size() != empty )
      {
          strQuery += " WHERE " ;

          for(int i=0; i< column_list_condition.size(); i++)
          {
              if ( column_list_condition.size() == 1 )
              {
                  strQuery += ( column_list_condition[i] + "=:" + column_list_condition[i] );
              }
              else if (column_list_condition.size()> 1 && i< column_list_condition.size() -1)
              {
                  strQuery += (column_list_condition[i] + "=:" + column_list_condition[i] +  " AND " );
              }
              else
              {
                  strQuery = (column_list_condition[i] + "=:" + column_list_condition[i]);
              }
          }
      }

      qDebug() << "database::update(): " <<strQuery;

      query.prepare(strQuery);

      for (int i = 0; i < column_list.size() ; i++)
      {
          query.bindValue(":"+column_list[i], value_list[i] );
      }

      for (int i = 0; i < column_list_condition.size() ; i++)
      {
          query.bindValue(":"+column_list_condition[i],value_list_condition[i] );
      }
    }
    else
    {
        return false;
    }

    isQueryExecuted = query.exec();

    if( !isQueryExecuted )
    {
        qDebug() << "database::update() query.exec() Error: " << query.lastError().text();
        qDebug() << "database error code: " << query.lastError().number();

        return isQueryExecuted;
    }

    return isQueryExecuted;

}



int Database::researcherLogin(QString userName, QString password)
{

    //int
    int count = 0;

    // creat instance of sql query
    QSqlQuery select;

    // selecting  values to connected db
    select.prepare("select * from user where username = '"+userName +"' and password = '"+password +"'");
    qDebug()<<"Executed?"<<select.lastQuery();
    //check if the select query executed successfully
    if(select.exec())
    {
        //- The select will loop through the database until the matching values are found
        while(select.next())
        {
            count ++;
        }
    }

    return count;

}


bool Database::registerResearcher(QString name, QString surname, QString username, QString password)
{
    // creat instance of sql query
    QSqlQuery insert;

    // inserting values to connected db
    insert.prepare("insert into user (name, surname, username,password) values ('"+ name +"', '"+ surname +"','"+ username +"','"+ password +"')");

    // check status of insertion
    if (insert.exec()){
        // - success
        return true;

    }
    else{
        // - failed
        return false;
    }

}

