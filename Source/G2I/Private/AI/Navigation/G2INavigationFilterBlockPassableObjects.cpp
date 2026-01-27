#include "G2INavigationFilterBlockPassableObjects.h"
#include "G2INavAreaBlockPassableObjects.h"

UG2INavigationFilterBlockPassableObjects::UG2INavigationFilterBlockPassableObjects()
{
	AddExcludedArea(UG2INavAreaBlockPassableObjects::StaticClass());
}
