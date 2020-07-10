#include "prototype.h"

struct confread_section *add_section(struct confread_file *file, char *name)
{

	// temp section pointer
	struct confread_section *thisSection = 0;
	struct confread_section *prevSection = 0;

	// ensure this section does not already exist
	if ((thisSection = confread_find_section(file, name))) {
		return thisSection;
	}
	// add the new section
	thisSection = malloc(sizeof(struct confread_section));

	// name the section
	thisSection->name = malloc(strlen(name) + 1);
	memcpy(thisSection->name, name, strlen(name) + 1);

	// set the initial values
	thisSection->next = 0;
	thisSection->pairs = 0;

	// get the first section in the list
	prevSection = file->sections;

	// special handling for first item
	if (!prevSection) {
		file->sections = thisSection;
		return thisSection;
	}
	// find the end of the linked list of sections
	while (prevSection && prevSection->next) {
		prevSection = prevSection->next;
	}

	// append to list
	prevSection->next = thisSection;

	return thisSection;

}

struct confread_pair *add_pair(struct confread_section *section, char *key,
			       char *value)
{

	// temp pair pointer
	struct confread_pair *thisPair = 0;
	struct confread_pair *prevPair = 0;

	// ensure pair does not exist
	if ((thisPair = confread_find_pair(section, key))) {

		// overwrite the value
		thisPair->value = realloc(thisPair->value, strlen(value) + 1);
		memcpy(thisPair->value, value, strlen(value) + 1);
		return thisPair;
	}
	// add the new pair
	thisPair = malloc(sizeof(struct confread_pair));

	// name the pair
	thisPair->key = malloc(strlen(key) + 1);
	memcpy(thisPair->key, key, strlen(key) + 1);

	// set the initial value
	thisPair->value = malloc(strlen(value) + 1);
	memcpy(thisPair->value, value, strlen(value) + 1);

	// set as end of list
	thisPair->next = 0;

	// get the first pair in the section
	prevPair = section->pairs;

	// special handling for first
	if (!prevPair) {
		section->pairs = thisPair;
		return thisPair;
	}
	// find the end of the linked list of pairs
	while (prevPair && prevPair->next) {
		prevPair = prevPair->next;
	}

	// add to list
	prevPair->next = thisPair;

	return thisPair;
}

struct confread_file *confread_open(char *path)
{

	// the actual configuration file
	FILE *confDataFile = 0;

	// temp pointers
	struct confread_file *confFile = 0;
	struct confread_section *thisSection = 0;

	// the line from the conf file
	char *line = 0;
	size_t lineLen = 0;

	// other char pointers for manipulation of the line
	char *lineStart = 0;
	char *lineEnd = 0;
	char *keyStart = 0;
	char *keyEnd = 0;

	// open the data file
	if (!(confDataFile = fopen(path, "r"))) {
		return 0;
	}
	// create the conf file
	confFile = malloc(sizeof(struct confread_file));
	confFile->name = malloc(strlen(path) + 1);
	confFile->sections = 0;
	memcpy(confFile->name, path, strlen(path) + 1);

	// add the 'root section'
	thisSection = add_section(confFile, "root");

	// Read the file line by line until EOF
	while (getline(&line, &lineLen, confDataFile) != -1) {

		// seek forward to the first non-space character
		for (lineStart = line; *lineStart != 0; ++lineStart) {
			if (!isspace(*lineStart)) {
				break;
			}
		}

		// omit if empty
		if (*lineStart == 0) {
			continue;
		}
		// omit if comment '#'
		if (*lineStart == '#') {
			continue;
		}
		// see if this is a section header
		if (*lineStart == '[') {

			// move lineStart off the header start
			++lineStart;

			// seek forward to the end of the header ']'
			for (lineEnd = lineStart; *lineEnd != 0; ++lineEnd) {
				if (*lineEnd == ']') {
					break;
				}
			}

			// if it's anything other than the end of header, invalid line
			if (*lineEnd != ']') {
				continue;
			}
			// seek linestart forward to trim whitespace
			for (lineStart = lineStart; lineStart != lineEnd;
			     ++lineStart) {
				if (!isspace(*lineStart)) {
					break;
				}
			}

			// omit zero size headers
			if (lineStart == lineEnd) {
				continue;
			}
			// seek line end back to trim whitespace
			for (lineEnd = lineEnd - 1; lineEnd != lineStart;
			     --lineEnd) {
				if (!isspace(*lineEnd)) {
					break;
				}
			}

			// set next char as null
			lineEnd[1] = 0;

			// create the new section
			thisSection = add_section(confFile, lineStart);

			continue;
		}
		// otherwise, key-value
		else {

			// seek line end forward to the separator '='
			for (lineEnd = lineStart; *lineEnd != 0; ++lineEnd) {
				if (*lineEnd == '=') {
					break;
				}
			}

			// discard if invalid
			if (*lineEnd != '=') {
				continue;
			}
			// if key is zero length, discard
			if (lineEnd == lineStart) {
				continue;
			}
			// set the key start
			keyStart = lineEnd + 1;

			// seek the line end backwards to trim whitespace
			for (lineEnd = lineEnd - 1; lineEnd != lineStart;
			     --lineEnd) {
				if (!isspace(*lineEnd)) {
					break;
				}
			}

			// set the following char to null
			lineEnd[1] = 0;

			// seek the key end forward to the end of the line
			for (keyEnd = keyStart; *keyEnd != 0; ++keyEnd) {
				if (*keyEnd == '\n') {
					break;
				}
			}

			// if the key is zero length, discard
			if (keyEnd == keyStart) {
				continue;
			}
			// seek key end backwards to trim trailing whitespace
			for (keyEnd = keyEnd; keyEnd != keyStart; --keyEnd) {
				if (!isspace(*keyEnd)) {
					break;
				}
			}

			// seek the key start forward to trim leading whitespace
			for (keyStart = keyStart; keyStart != keyEnd;
			     ++keyStart) {
				if (!isspace(*keyStart)) {
					break;
				}
			}

			// set the new line char to null
			keyEnd[1] = 0;

			// add the key-value pair
			add_pair(thisSection, lineStart, keyStart);

		}
	}

	free(line);
	fclose(confDataFile);

	return confFile;

}

struct confread_section *confread_find_section(struct confread_file *confFile,
					       char *name)
{

	// temp pointer to section
	struct confread_section *thisSection = 0;

	// null check
	if (!confFile || !name) {
		return 0;
	}
	// set as first section
	thisSection = confFile->sections;

	// loop through all sections
	while (thisSection) {

		// check for match
		if (!strcmp(thisSection->name, name)) {
			break;
		}
		// next section
		thisSection = thisSection->next;

	}

	// return found section or null
	return thisSection;

}

struct confread_pair *confread_find_pair(struct confread_section *confSec,
					 char *key)
{

	// temp pair pointer
	struct confread_pair *thisPair = 0;

	// null check
	if (!confSec || !key) {
		return 0;
	}
	// set to start of pairs
	thisPair = confSec->pairs;

	// iterate through all pairs in section
	while (thisPair) {

		// check if the key matches target key
		if (!strcmp(thisPair->key, key)) {
			break;
		}
		// get next pair
		thisPair = thisPair->next;

	}

	// return found pair or null
	return thisPair;

}

char *confread_find_value(struct confread_section *confSec, char *key)
{

	struct confread_pair *thisPair = 0;

	// get the pair
	if (!(thisPair = confread_find_pair(confSec, key))) {
		return 0;
	}

	return thisPair->value;

}

void confread_close(struct confread_file **confFile)
{

	// temp section pointer
	struct confread_section *thisSection = 0;
	struct confread_section *nextSection = 0;

	// temp pair pointer
	struct confread_pair *thisPair = 0;
	struct confread_pair *nextPair = 0;

	// null check
	if (!confFile || !(*confFile)) {
		return;
	}
	// get the first section
	thisSection = (*confFile)->sections;

	// loop through all sections in the file
	while (thisSection) {

		// get the first pair in this section
		thisPair = thisSection->pairs;

		// loop through each key pair in the section
		while (thisPair) {

			// get the next pointer before freeing anything
			nextPair = thisPair->next;

			// free the two data elements in the pair
			free(thisPair->key);
			free(thisPair->value);

			// free the pair itself
			free(thisPair);

			// move to the next one
			thisPair = nextPair;

		}

		// get the next pointer before freeing anything
		nextSection = thisSection->next;

		// all pairs have been freed, now free the section data
		free(thisSection->name);

		// free the section itself
		free(thisSection);

		// move to the next one
		thisSection = nextSection;

	}

	// free the file section data
	free((*confFile)->name);

	// free the conf file itself
	free(*confFile);
	*confFile = 0;

}

int confread_check_pair(struct confread_section *section, char *key, char *value){

	char *thisValue = 0;

	// sanity checks
	if(!section || !key || !value){
		return 0;
	}

	// attempt to find the value
	thisValue = confread_find_value(section, key);

	if(!thisValue){
		return 0;
	}

	return (!strcmp(value, thisValue));
}




conf_file_t * getConf(){
	conf_file_t * globalConfRead = malloc(sizeof(conf_file_t));

	globalConfRead->ip_ftp = malloc(25);
	globalConfRead->port_ftp = malloc(10);
	globalConfRead->user_ftp = malloc(25);
	globalConfRead->public_keyfile = malloc(40);
	globalConfRead->private_keyfile = malloc(40);
	globalConfRead->dest_path = malloc(70);
	globalConfRead->output_name = malloc(30);
	globalConfRead->upload_protocol = malloc(10);
	globalConfRead->ip_sql = malloc(25);
	//globalConfRead->port_sql = malloc(10);
	globalConfRead->user_sql = malloc(25);
	globalConfRead->pwd_sql = malloc(50);
	globalConfRead->table_name = malloc(25);

	// Get conf touchs
	struct confread_file *configFile;
	if(!(configFile = confread_open("curl.conf"))){
		fprintf(stderr, "Config open failed\n");
		exit(EXIT_FAILURE);
	};

	//load globalConf struct
	struct confread_section* curlConf = confread_find_section(configFile, "curl");
	strcpy(globalConfRead->ip_ftp, confread_find_value(curlConf,"ip"));
	strcpy(globalConfRead->port_ftp, confread_find_value(curlConf,"port"));
	strcpy(globalConfRead->user_ftp, confread_find_value(curlConf,"user"));
	strcpy(globalConfRead->public_keyfile, confread_find_value(curlConf,"public_keyfile"));
	strcpy(globalConfRead->private_keyfile, confread_find_value(curlConf,"private_keyfile"));
	strcpy(globalConfRead->dest_path, confread_find_value(curlConf,"dest_path"));
	strcpy(globalConfRead->output_name, confread_find_value(curlConf,"output_name"));
	strcpy(globalConfRead->upload_protocol, confread_find_value(curlConf,"upload_protocol"));

	struct confread_section* sqlConf = confread_find_section(configFile, "mysql");
	strcpy(globalConfRead->ip_sql, confread_find_value(sqlConf,"ip"));
	globalConfRead->port_sql = atoi(confread_find_value(sqlConf,"port"));
	strcpy(globalConfRead->user_sql,confread_find_value(sqlConf,"user"));
	strcpy(globalConfRead->pwd_sql,confread_find_value(sqlConf,"pwd"));
	strcpy(globalConfRead->table_name,confread_find_value(sqlConf,"table_name"));

	
	  confread_close(&configFile);
	  return globalConfRead;
}

void * cleanConfFile(conf_file_t * globalConfRead){
	free(globalConfRead->ip_ftp);
	free(globalConfRead->port_ftp);
	free(globalConfRead->user_ftp);
	free(globalConfRead->public_keyfile);
	free(globalConfRead->private_keyfile);
	free(globalConfRead->dest_path);
	free(globalConfRead->output_name);
	free(globalConfRead->upload_protocol);
	free(globalConfRead->ip_sql);
	free(globalConfRead->user_sql);
	free(globalConfRead->pwd_sql);
	free(globalConfRead->table_name);
	free(globalConfRead);
}