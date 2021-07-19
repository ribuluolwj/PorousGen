#!/bin/bash
rm -vf *.exp *.ilk *.lib *.pdb *.vb
mv -vf /d/Program\ Files/Siemens/NX1899/MYPROGRAM/Application/*.* /d/Program\ Files/Siemens/NX1899/MYPROGRAM/Application/bak/
cp -vf *.dll *.dlx /d/Program\ Files/Siemens/NX1899/MYPROGRAM/Application
cp -vf *.dll *.dlx ../UG_DEV_Update_for_Turbine/Application
cp -vf ../Startup/*.* ../UG_DEV_Update_for_Turbine/Startup
