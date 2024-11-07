#ifndef OBJFILES_H
#define OBJFILES_H

#include "RayCasting.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <map>

class Parser {
public:
	std::vector<Entity> vertices = {
		CreatePoint(0.0f, 0.0f, 0.0f)
	};
    std::string lineIgnored;

	std::map<std::string, Group*> groupNames;
};

Parser Parse_Obj_File(std::ifstream& file);
Group* Obj_To_Group(Parser parser);

#endif // !OBJFILES_H
