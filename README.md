### **Instructions for DealerDrive to run in docker:**

Run docker container with:

*docker run –rm -ti -p port:port -v <your local directory>:/SharedDocker dockerImage*



Navigate to ShareDocker/backend/build folder:

For first time use:

*cmake ../..* 	# CMakeLists.txt is on the root directory(sharedDocker).



Every time there is changes to code base:

run *mak*e command



This will generate an executable: *Main*

Run executable with:

*./Main* 

