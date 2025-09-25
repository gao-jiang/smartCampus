#include "SmartCampusAssistant.h"
#include "SmartCampusMenu.h"

int main() {
    SmartCampusAssistant assistant("data");
    SmartCampusMenu menu(assistant);
    menu.run();
    return 0;
}
