#include "OBJFiles.h"

Parser Parse_Obj_File(std::ifstream& file) {
    Parser parser;
    if (!file) {
        std::cout << "File does not exist" << std::endl;
        return parser;
    }

    std::string line, g_name;
    while (std::getline(file, line)) {
        std::istringstream stream(line);

        if (line[0] == 'v' && (line[1] == ' ' || line[1] == '\t')) {
            std::vector<float> position;
            float coordinate;

            // Skip the 'v' character in the stream
            stream.ignore(1);

            // Extract floats from the line
            while (stream >> coordinate) {
                position.push_back(coordinate);
            }

            // Check that we have exactly 3 coordinates before adding the vertex
            if (position.size() == 3) {
                parser.vertices.push_back(CreatePoint(position[0], position[1], position[2]));
            }
        }
        else if (line[0] == 'f' && (line[1] == ' ' || line[1] == '\t')) {
            std::vector<int> list_vertices{ 0 };
            int order;

            stream.ignore(1);

            while (stream >> order)
                list_vertices.push_back(order);

            if (list_vertices.size() >= 3) {
                for (int i = 2; i < list_vertices.size() - 1; i++) {
                    Triangle* triangle = new Triangle(parser.vertices[list_vertices[1]], parser.vertices[list_vertices[i]], parser.vertices[list_vertices[i + 1]]);
                    triangle->mat.color = Color(0.85f, 0.0f, 0.0f);
                    AddChild(parser.groupNames[g_name], triangle);
                }
            }
        }
        else if (line[0] == 'g' && (line[1] == ' ' || line[1] == '\t')) {
            Group* new_group = new Group();
            
            stream.ignore(1);

            while (stream >> g_name)
                new_group->ID = new_group->ID + g_name;

            parser.groupNames[g_name] = new_group;
        }
        else {
            parser.lineIgnored += line + "\n";
        }
    }

    return parser;
}

Group* Obj_To_Group(Parser parser) {
    Group* group = new Group();

    std::map<std::string, Group*>::iterator iter = parser.groupNames.begin();

    while (iter != parser.groupNames.end()) {
        AddChild(group, iter->second);
        ++iter;
    }

    return group;
}