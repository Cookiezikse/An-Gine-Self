#include "GameObject/Components/Transform.h"

#include <iostream>


void Transform::Display()
{
	std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")\n";
	std::cout << "Rotation: (" << rotation.x << ", " << rotation.y << ", " << rotation.z << " )\n";
	std::cout << "Scale: (" << scale.x << ", " << scale.y << ", " << scale.z << ")\n";
}
