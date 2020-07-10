#include <string.h>
#include <my_global.h>
#include <mysql.h>	
#include "prototype.h"

extern conf_file_t * globalConfRead; 

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);        
}



MYSQL * connectDB(){

  MYSQL * con = mysql_init(NULL);
  
  if (con == NULL)
  {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }

  if (mysql_real_connect(con, globalConfRead->ip_sql, globalConfRead->user_sql, globalConfRead->pwd_sql, globalConfRead->table_name, globalConfRead->port_sql, NULL, 0) == NULL){ 
        finish_with_error(con);
  }   
  mysql_set_character_set(con, "utf8");

  return con;
}


int execRequest(MYSQL_STMT * req_prep_Check){
 // On exécute la requête
  if (mysql_stmt_execute(req_prep_Check) != 0)
  {
    printf("Impossible d'exécuter la requête //Check User//\n");
    return 0;
  }
 
  if(mysql_stmt_fetch(req_prep_Check) != MYSQL_NO_DATA){
    printf("L'utilisateur a bien été créé\n");
    return 1;
  }
  return 0;
}

int checkUser(char * mail){
  MYSQL * con = connectDB();


  /* Vérification que le user n'est pas déjà en base */
  MYSQL_STMT  * req_prep_Check =  mysql_stmt_init(con);

  char * checkRequest = "select idUser from USER where emailAddress = ?";
    if (mysql_stmt_prepare(req_prep_Check, checkRequest, strlen(checkRequest)) != 0){
      printf("Impossible de préparer la requête Check user Local\n");
    return 0;
  }  


  MYSQL_BIND paramCheck[1];

  memset((void*) paramCheck, 0, sizeof(paramCheck));

  paramCheck[0].buffer_type = MYSQL_TYPE_STRING;
  paramCheck[0].buffer = (char *) mail;
  paramCheck[0].buffer_length= strlen(mail);
  paramCheck[0].is_unsigned = 0;
  paramCheck[0].is_null = 0;

  // On lie les paramUserètres
  if (mysql_stmt_bind_param(req_prep_Check, paramCheck) != 0)
  {
    printf("Impossible de lier les paramètres à la requête  //Check User//\n");
    return 0;
  }
  int timer = 0;
  int result;
  do{
    result = execRequest(req_prep_Check);
    timer++;
    printf("timer: %d\n",timer);
    sleep(0.75);

  }while(!result && timer < 6 );

  return result;

}






