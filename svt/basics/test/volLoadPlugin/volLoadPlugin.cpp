#include <svt_volLoadPlugin.h>
#include <svt_iostream.h>

int main()
{
    svt_volLoadPlugin oPlugin;

    if (!oPlugin.loadPlugin("demo"))
    {
	cout << "volLoadPlugin: cannot load plugin!" << endl;
        exit(1);
    }

    if (oPlugin.getExtensionDesc())
	cout << oPlugin.getExtensionDesc() << endl;

    return  0;
}


