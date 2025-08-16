#include "BTU.h"
#include "University/University.h"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>


// Global University B-TU ;-)
University btu("B-TU");

// Buffer for the imprint.
char imprint[128];


// btu.cpp
// usage:
//	-> btu add Name LastName ID Passwort
//  -> btu remove ID passwort
//	-> btu print
int main(int argc, char** argv)
{
	if(argc < 2)
	{
		std::cout << "usage (Adding a new student): btu add Name Last_Name ID Password\n"
				  << "usage (Removing a student):   btu remove ID Password\n"
				  << "usage (Printing all records): btu print"
				  << std::endl;
		std::cout << "Please note: Students can only be exmatriculated using the same password that was used to imatriculate them." << std::endl;
		return 0;
	}

	// Prepare the imprint, since every good software has one!
	// Should not be longer than 128 Bytes.
	strcpy(imprint,
		"This is B-TU Student management System V1.0"
		"\nAll rights are reserved by B-TU Management"
		"\nCopyright 2020 - ALL ETERNITY"
	);

	// Write imprint to console
	std::cout << imprint << std::endl;

	// Add new student
	if(strcmp(argv[1], OPTION_ADD) == 0)
	{
		if(argc != 6)
		{
			std::cout << "usage: btu add Name LastName ID Passwort" << std::endl;
			return 0;
		}
		std::cout << "Adding Student to database..." << std::endl;
		btu.add_student(argv[2], argv[3], std::stoul(argv[4],nullptr,0), argv[5], true);
	}

	// Remove a student from the database
	if(strcmp(argv[1], OPTION_REMOVE) == 0)
	{
		if(argc != 4)
		{
			std::cout << "usage: btu remove ID pass" << std::endl;
			return 0;
		}
		std::cout << "Removing Student from database..." << std::endl;
		btu.request_exmatriculation(std::stoul(argv[2], nullptr,0), argv[3]);
	}

	// Print the data of all enrolled Students
	if(strcmp(argv[1], OPTION_STATISTICS) == 0)
	{
		std::cout << "Printing Statistics...\n" << std::endl;
		btu.print_records();
	}

	return 0;
}
