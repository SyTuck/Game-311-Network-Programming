#pragma once

class NetworkManager
{

public:
	static NetworkManager* GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new NetworkManager();
		}
		return instance;
	}

	void Init() {};

private:
	NetworkManager();
	~NetworkManager();

	//singleton instance
	static NetworkManager* instance;

};

