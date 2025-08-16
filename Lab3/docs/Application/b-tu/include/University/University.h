#ifndef __UNIVERSITY_H__
#define __UNIVERSITY_H__

/**
 * Includes:
 */
#include <map>
#include <string>

/**
 * Prototypes:
 */
struct Student;


/**
 * @class Basic class to represent a University.
 */
class University
{
	public:

		/**
		 * Creates a new University Instance.
		 *
		 * @param name Name of the University
		 */
		University(const std::string name) : name(name), db_path(name + ".db")
		{
			initialize();
		}

		/**
		 * Default Destructor. Writes out data on
		 * object deletion and free's used memory.
		 */
		~University()
		{
			// Side Effect: flush internal data
			// to the hard drive
			close();
		}

		/**
		 * Add Student to university, i.e. imatricutate.
		 *
		 * @param name Name of the Student
		 * @param last_name Last Name of the Student
		 * @param id Student ID
		 * @param password Password used by the Student
		 * @param Notify the Student that he was added
		 */
		void add_student(const char *const name,
						 const char *const last_name,
						 const unsigned int id,
						 const char *const password,
						 const bool notify = false);

		/**
		 * Initialize the underlying Database where Student
		 * recordds are Stored.
		 */
		void initialize();

		/**
		 * Write out Student records to the Database file.
		 */
		void flush();

		/**
		 * Close the University. :-)
		 */
		void close();

		/**
		 * Print out all Student Records, each with Name,
		 * Last Name and id. Due to privacy reasons, the students
		 * password is omitted.
		 */
		void print_records();

		/**
		 * Request the Examtriculation of a Student. This
		 * operation requires the Student's ID and his/her
		 * password for validation.
		 *
		 * @param id Student ID
		 * @param password Password of the Student
		 */
		void request_exmatriculation(const unsigned int id, const char* const password);

		/**
		 * Returns the Name of the University.
		 *
		 * @return Name of the University.
		 */
		const std::string get_name() const
		{
			return name;
		}


	protected:

		/**
		 * Private Helper Function to exmatriculate a
		 * Student specified by his/her Student ID.
		 */
		void exmatriculate(const unsigned int id);

		/**
		 * Notify the Student with the given id
		 * about it's immatriculation.
		 */
		void notifyStudentOnImatriculation(const unsigned int id);

		/**
		 * Notify the Student with the given id
		 * about it's sucessfull imatriculation.
		 */
		void notifyStudentOnExmatriculation(const unsigned int id);

	private:
		std::string name;
		std::string db_path;
		std::map<const unsigned int, Student*> student_records;
};

#endif
