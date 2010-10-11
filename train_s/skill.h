#include <vector>
#include <map>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef SKILL
#define SKILL

class skill_rec;
#define PERIOD_DAY 24*60*60 // one day in seconds

class Skill;

struct corr_skill_rec {
	Skill *skill;
	float correl;
};

class Skill {
private:
	char *name;
	char *description;
	
	corr_skill_rec parent_skill; // - parent skill, with correlation (part of parent)
	std::vector<Skill *> child_skills; // - child skills, to delete
	std::vector<corr_skill_rec> correlations; // - correlated skills

	time_t creation_time;

protected:
	std::map<time_t, skill_rec> data;
	
	virtual int save_self(FILE *file, int &num);
	virtual int save_self_data(FILE *file, int &num);
public:
	Skill( const char *name, const char *description, float importance );
	~Skill();

	static const char *class_type;
	const char *type;
	
	float importance_for_now;
	virtual const char *get_name() {return name;}
	virtual const char *get_description() {return description;}

	void set_parent(Skill *, float correl);
	void add_child(Skill *);
	void add_correl(Skill *, float correl);
	Skill **make_child_array(int *size);
	virtual int get_max();
	virtual int get_amount();
	virtual int advance( unsigned char amount, const char *description, unsigned int period = PERIOD_DAY );  // -1 - error
	time_t get_creation_time() { return creation_time; }
	
	
	friend int save_skills(std::multimap<float, Skill*> *cont, const char *file);
	friend int load_skills(std::multimap<float, Skill*> *cont, const char *file);
};



class skill_rec {
public:
	~skill_rec();
	unsigned char advance;
	unsigned int period;
	char *description;
	int save_self(FILE *file, Skill *skill_id);
};

#endif
