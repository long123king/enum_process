#ifndef __ENUM_PROCESS_PDH_TEST_H__
#define __ENUM_PROCESS_PDH_TEST_H__

class PdhTest
{
public:
	static PdhTest* GetInstance()
	{
		static PdhTest _instance;
		return &_instance;
	}

	void testHelp();
	void testCounter();
	void test();

private:
	PdhTest();
	~PdhTest();
};

#endif//__ENUM_PROCESS_PDH_TEST_H__