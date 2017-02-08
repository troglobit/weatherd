

struct wdb {
	int id;

	float pressure;
	float temperature;
	float humidity;
	float illuminance;
	float infrared;

	char timestamp[20];
};

int db_put(struct wdb *entry);
int db_get(struct wdb *entry);

