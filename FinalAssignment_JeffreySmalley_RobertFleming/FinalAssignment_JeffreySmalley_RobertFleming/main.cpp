#include <Ogre.h>
int main(int argc, char* argv[])
{
    Ogre::Root* root = new Ogre::Root("plugins.cfg", "ogre.cfg", "Ogre.log");
    delete root;
    system("pause");
}