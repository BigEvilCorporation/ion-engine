#include "IOTest.h"
#include "core/Version.h"
#include "core/debug/Debug.h"
#include "core/thread/Thread.h"
#include "core/time/Time.h"

#include <sstream>

IOTest::IOTest()
: ion::framework::Application("ion::engine I/O Test")
{
	mFileSystem = NULL;
}

IOTest::~IOTest()
{
}

bool IOTest::Initialise()
{
	std::stringstream welcomeMsg;
	welcomeMsg << "ion::engine - build " << ion::sVersion.Major << "." << ion::sVersion.Minor << "." << ion::sVersion.Build;
	ion::debug::Log(welcomeMsg.str().c_str());

	//Create filesystem
	mFileSystem = new ion::io::FileSystem();

	//Get default file device
	ion::io::FileDevice* defaultFileDevice = mFileSystem->GetDefaultFileDevice();

	//Get current directory
	std::string currentDirectory = defaultFileDevice->GetDirectory();

	//Read directory
	std::vector<ion::io::FileDevice::DirectoryItem> directoryItems;
	defaultFileDevice->ReadDirectory(currentDirectory, directoryItems);

	//Run tests
	TestBasicSerialisation();
	//TestVersionedSerialisation();
	
	//No updating/rendering required, exit app
	return true;
}

void IOTest::TestBasicSerialisation()
{
	//Create source object
	TestSerialisable sourceObject;

	//Fill with known data
	sourceObject.mTestInt = 12345;
	sourceObject.mTestFloat = 2.0f;
	sourceObject.mTestString1 = "Hello ";
	sourceObject.mTestString2 = " world!";
	sourceObject.mTestIntBlock1 = 5;
	sourceObject.mTestIntBlock2 = 6;
	sourceObject.mTestIntBlock3 = 7;
	sourceObject.mTestIntBlock4 = 8;
	sourceObject.mTestIntBlock5 = 9;
	sourceObject.mTestIntBlock6 = 10;
	sourceObject.mTestIntArray.push_back(67890);
	sourceObject.mTestIntArray.push_back(98765);
	sourceObject.mTestFloatArray.push_back(3.14195f);
	sourceObject.mTestFloatArray.push_back(3.0f);
	sourceObject.mTestIntList.push_back(10203040);
	sourceObject.mTestIntList.push_back(50607080);
	sourceObject.mTestIntMap.insert(std::pair<std::string, int>("TestMapEntry1", 11111));
	sourceObject.mTestIntMap.insert(std::pair<std::string, int>("TestMapEntry2", 22222));
	sourceObject.mTestSubClass.mTestInt = 43210;
	sourceObject.mTestSubClass.mTestFloat = 4.0f;
	sourceObject.mTestSubClass.mTestString = "SubClass test string";
	sourceObject.mTestSubClassPtrBase = new TestSerialisable::SubClass();
	sourceObject.mTestSubClassPtrDerived1 = new TestSerialisable::SubClassDerived();
	sourceObject.mTestSubClassPtrDerived2 = new TestSerialisable::SubClassDerived();
	sourceObject.mTestSubClassPtrDerived3 = new TestSerialisable::SubClassDerivedSerialiseAsBase();
	sourceObject.mNullPtr = NULL;

	sourceObject.mTestSubClassPtrBase->mTestInt = 1;
	sourceObject.mTestSubClassPtrBase->mTestIntV2 = 2;
	sourceObject.mTestSubClassPtrBase->mTestFloat = 3.14195f;

	sourceObject.mTestSubClassPtrDerived1->mTestInt = 3;
	sourceObject.mTestSubClassPtrDerived1->mTestIntV2 = 4;
	sourceObject.mTestSubClassPtrDerived1->mTestFloat = 9876.5f;
	sourceObject.mTestSubClassPtrDerived1->mTestString = "TestSubClassDerived1";

	sourceObject.mTestSubClassPtrDerived2->mTestInt = 5;
	sourceObject.mTestSubClassPtrDerived2->mTestIntV2 = 6;
	sourceObject.mTestSubClassPtrDerived2->mTestFloat = 5678.9f;
	sourceObject.mTestSubClassPtrDerived2->mTestString = "TestSubClassDerived2";

	sourceObject.mTestSubClassPtrDerived3->mTestInt = 7;
	sourceObject.mTestSubClassPtrDerived3->mTestIntV2 = 8;
	sourceObject.mTestSubClassPtrDerived3->mTestFloat = 101010.1f;

	const char* filename = "ion_serialise_test.bin";

	//Create and open a file stream for writing
	ion::io::File fileOut;
	if(!fileOut.Open(filename, ion::io::File::OpenWrite))
	{
		ion::debug::Error("Error opening file for writing");
	}

	//Create an archive for serialising out
	ion::io::Archive archiveOut(fileOut, ion::io::Archive::Out, NULL);

	//Serialise
	archiveOut.Serialise(sourceObject);

	//Close file
	fileOut.Close();

	//Create destination object
	TestSerialisable destinationObject;

	//Create and open a file stream for reading
	ion::io::File fileIn;
	if(!fileIn.Open(filename, ion::io::File::OpenRead))
	{
		ion::debug::Error("Error opening file for reading");
	}

	//Create an archive for serialising in
	ion::io::Archive archiveIn(fileIn, ion::io::Archive::In, NULL);

	//Serialise
	archiveIn.Serialise(destinationObject);

	//Close file
	fileIn.Close();

	//Test
	if(sourceObject == destinationObject)
	{
		ion::debug::Log("Source and destination objects match - serialisation test passed");
	}
	else
	{
		ion::debug::Error("Source and destination objects mismatch - serialisation test failed");
	}

	//Clean up
	delete sourceObject.mTestSubClassPtrBase;
	delete sourceObject.mTestSubClassPtrDerived1;
	delete sourceObject.mTestSubClassPtrDerived2;
}

void IOTest::TestVersionedSerialisation()
{
	//Create source object
	TestSerialisable sourceObject;

	//Fill with known data
	sourceObject.mTestInt = 12345;
	sourceObject.mTestFloat = 2.0f;
	sourceObject.mTestString1 = "Hello ";
	sourceObject.mTestString2 = " world!";
	sourceObject.mTestIntArray.push_back(67890);
	sourceObject.mTestIntArray.push_back(98765);
	sourceObject.mTestFloatArray.push_back(3.14195f);
	sourceObject.mTestFloatArray.push_back(3.0f);
	sourceObject.mTestIntList.push_back(102030);
	sourceObject.mTestIntList.push_back(405060);
	sourceObject.mTestIntMap.insert(std::pair<std::string, int>("TestMapItem1", 111111));
	sourceObject.mTestIntMap.insert(std::pair<std::string, int>("TestMapItem2", 222222));
	sourceObject.mTestSubClass.mTestInt = 43210;
	sourceObject.mTestSubClass.mTestFloat = 4.0f;
	sourceObject.mTestSubClass.mTestString = "SubClass test string";
	sourceObject.mTestSubClassPtrBase = new TestSerialisable::SubClass();
	sourceObject.mTestSubClassPtrDerived1 = new TestSerialisable::SubClassDerived();
	sourceObject.mTestSubClassPtrDerived2 = new TestSerialisable::SubClassDerived();
	sourceObject.mTestSubClassPtrDerived3 = new TestSerialisable::SubClassDerivedSerialiseAsBase();
	sourceObject.mNullPtr = NULL;

	sourceObject.mTestSubClassPtrBase->mTestInt = 1;
	sourceObject.mTestSubClassPtrBase->mTestIntV2 = 2;
	sourceObject.mTestSubClassPtrBase->mTestFloat = 3.14195f;

	sourceObject.mTestSubClassPtrDerived1->mTestInt = 3;
	sourceObject.mTestSubClassPtrDerived1->mTestIntV2 = 4;
	sourceObject.mTestSubClassPtrDerived1->mTestFloat = 9876.5f;
	sourceObject.mTestSubClassPtrDerived1->mTestString = "TestSubClassDerived1";

	sourceObject.mTestSubClassPtrDerived2->mTestInt = 5;
	sourceObject.mTestSubClassPtrDerived2->mTestIntV2 = 6;
	sourceObject.mTestSubClassPtrDerived2->mTestFloat = 5678.9f;
	sourceObject.mTestSubClassPtrDerived2->mTestString = "TestSubClassDerived2";

	sourceObject.mTestSubClassPtrDerived3->mTestInt = 7;
	sourceObject.mTestSubClassPtrDerived3->mTestIntV2 = 8;
	sourceObject.mTestSubClassPtrDerived3->mTestFloat = 101010.1f;

	const char* filenameV1 = "ion_serialise_test_v1.bin";
	const char* filenameV2 = "ion_serialise_test_v2.bin";

	//Create and open file streams for writing
	ion::io::File fileOutV1;
	ion::io::File fileOutV2;

	if(!fileOutV1.Open(filenameV1, ion::io::File::OpenWrite))
	{
		ion::debug::Error("Error opening file for writing");
	}

	if(!fileOutV2.Open(filenameV2, ion::io::File::OpenWrite))
	{
		ion::debug::Error("Error opening file for writing");
	}

	//Create archives for serialising out
	ion::io::Archive archiveOutV1(fileOutV1, ion::io::Archive::Out, NULL);
	ion::io::Archive archiveOutV2(fileOutV2, ion::io::Archive::Out, NULL);

	//Serialise
	sourceObject.Serialise(archiveOutV1);
	sourceObject.Serialise(archiveOutV2);

	//Close files
	fileOutV1.Close();
	fileOutV2.Close();

	//Create destination objects
	TestSerialisable destinationObjectV1;
	TestSerialisable destinationObjectV2;

	//Create and open file streams for reading
	ion::io::File fileInV1;
	ion::io::File fileInV2;

	if(!fileInV1.Open(filenameV1, ion::io::File::OpenRead))
	{
		ion::debug::Error("Error opening file for reading");
	}

	if(!fileInV2.Open(filenameV2, ion::io::File::OpenRead))
	{
		ion::debug::Error("Error opening file for reading");
	}

	//Create archives for serialising in
	ion::io::Archive archiveInV1(fileInV1, ion::io::Archive::In, NULL);
	ion::io::Archive archiveInV2(fileInV2, ion::io::Archive::In, NULL);

	//Serialise
	destinationObjectV1.Serialise(archiveInV1);
	destinationObjectV2.Serialise(archiveInV2);

	//Close files
	fileInV1.Close();
	fileInV2.Close();

	//Test
	if(sourceObject.Test(destinationObjectV1, 1))
	{
		ion::debug::Log("Source and destination objects match - serialisation test passed");
	}
	else
	{
		ion::debug::Error("Source and destination objects mismatch - serialisation test failed");
	}

	if(sourceObject.Test(destinationObjectV2, 2))
	{
		ion::debug::Log("Source and destination objects match - serialisation test passed");
	}
	else
	{
		ion::debug::Error("Source and destination objects mismatch - serialisation test failed");
	}

	//Clean up
	delete sourceObject.mTestSubClassPtrBase;
	delete sourceObject.mTestSubClassPtrDerived1;
	delete sourceObject.mTestSubClassPtrDerived2;
}

void IOTest::Shutdown()
{
	if(mFileSystem)
		delete mFileSystem;
}

bool IOTest::Update(float deltaTime)
{
	return false;
}

void IOTest::Render()
{
}