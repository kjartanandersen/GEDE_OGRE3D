#pragma once
#pragma once


	class ICallback
	{
	public:
		virtual ~ICallback() {}

		virtual void callback(void* item) = 0;
	};



