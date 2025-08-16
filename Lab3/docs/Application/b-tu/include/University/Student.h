#ifndef __STUDENT_H__
#define __STUDENT_H__

/**
 * Basic Struct to represent a Student.
 */
struct Student
{
	/**
	 * Initialize a new Student by clearing out all
	 * data.
	 */
	Student();

	/**
	 * Maximum allowed length for Passwords.
	 */
	const static unsigned int MAX_PASSWORD_LENGTH = 32;

	/**
	 * Password used to log in at University. The students
	 * passwords are allowed to be up to MAX_PASSWORD_LENGTH
	 * characters.
	 */
	char password[MAX_PASSWORD_LENGTH];

	/**
	 * Student's identification number.
	 */
	unsigned int id;

	/**
	 * Last Name of the Student. Will be allocated dynamically
	 * to support different Name length.
	 */
	char* last_name;

	/**
	 * Name of the Student. Will be allocated dynamically
	 * to support different Name length.
	 */
	char* name;
};

#endif
