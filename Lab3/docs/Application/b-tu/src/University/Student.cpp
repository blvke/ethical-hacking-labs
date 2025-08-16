#include "University/Student.h"
#include <string.h>

Student::Student()  : id(0), last_name(0), name(0) 
{
	// clear out password data
	memset(password, 0, Student::MAX_PASSWORD_LENGTH);
}
