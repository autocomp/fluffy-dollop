#ifndef ENTITYBOOKMARK_H
#define ENTITYBOOKMARK_H

#include <iostream>
#include <map>
#include <string>
#include <QSettings>
using namespace std;

class NotFolderException {};

class Entity {

    protected:
    string name;

    public:
    Entity(string &nm):name(nm) {}
    string &getName(){return name;}
//    void setName(string &nm) {name = nm;}

    virtual bool addEntity(Entity *) { throw NotFolderException();}
    virtual Entity* findEntity(string) { throw NotFolderException();}
    virtual Entity* findEntityRoot(string) { throw NotFolderException();}
    virtual bool isFolder() = 0;
};

class Bmark: public Entity {
    string payload;

    float m_pitch, m_yaw, m_roll;
    double m_x, m_y, m_z;
    int m_idNavigation;

    public:

    void setAngle(float pitch, float yaw, float roll) {
        m_pitch = pitch;
        m_yaw = yaw;
        m_roll = roll;
    }

    void setOrigin(double x, double y, double z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    void setNavigation(int id){
        m_idNavigation = id;
    }

//    void setName(string &nm) {name = nm;}

    float getPitch(){return m_pitch;}
    float getYaw(){return m_yaw;}
    float getRoll(){return m_roll;}

    double getX(){return m_x;}
    double getY(){return m_y;}
    double getZ(){return m_z;}

    int getIdNavigation() {return m_idNavigation;}

    bool isFolder() {return false;}
    string getPayload() { return payload; }
    void setPayload(string newpayload) { payload = newpayload; }
    Bmark(string nm, string pl):Entity(nm), payload(pl) {}

    Bmark(string nm, double c1,double c2,double c3,
             float c4,float c5,float c6, int id):
        Entity(nm), m_x(c1),m_y(c2),m_z(c3),m_pitch(c4),m_yaw(c5),m_roll(c6),m_idNavigation(id)
    {
        setAngle(c4,c5,c6);
        setOrigin(c1,c2,c3);
        setNavigation(id);
    }

       const char* getCamStr() {
           static char camstr[256];
           sprintf(camstr,"%f %f %f %f %f %f %d\n", m_x,m_y,m_z,m_pitch,m_yaw,m_roll,m_idNavigation);
           return camstr;
       }
};
typedef map <string, Entity*> ETree;
typedef ETree::iterator EIt;

class Folder: public Entity {

    ETree e_tree;

    Entity *getLeaf(string nm) { // returns direct leaf
        return e_tree[nm]; // should always succeed, if not there is error in implementation
    }

    bool eraseLeaf(string nm) { // erase leaf from current branch
        e_tree.erase(nm);
    }

    string ms(int level) {
        string rs;
        for (int i = 0; i < level*4; i++) {
            rs += " ";
        }
        return rs;
    }

    public:

    ETree * getTree(){return &e_tree;}
    bool isFolder() {return true;}

//    void setName(string &nm) {name = nm;}
    bool addEntity(Entity *ne) { // adds Entity to folder
        e_tree[ne->getName()] = ne;
    }


    Entity *findEntity(string nm) { // finds entity in tree. returns NULL if
                                    // there isn't one
        Folder* root_ent = findEntityRoot(nm);
        if (root_ent)
            return root_ent->getLeaf(nm);
        else
            return NULL;
    }

    bool deleteEntity(string nm) { // return false if no such entity found
        Folder *root_ent = dynamic_cast<Folder *>(findEntityRoot(nm));

        if (!root_ent)
            return false;

        root_ent->eraseLeaf(nm);
        return true;
    }

    Folder *findEntityRoot(string nm) { // returns NULL if not found
        EIt ir;
        Entity* res;

        if ((ir = e_tree.find(nm)) == e_tree.end()) {
            // name is not in current branches, search recursively
            for (EIt it = e_tree.begin(); it != e_tree.end(); it++) {
                if (it->second->isFolder() &&
                    (res = it->second->findEntityRoot(nm)))
                    return dynamic_cast<Folder *>(res); // found entity down
                                                        // the road
            }

            return NULL;        // no such entry in subtrees
        }
        else {
            return this;        // found entity in leafs of current branch
        }
    }


    void dumpTree(int level = 1) { // insert your saving code here
        cout << ms (level-1) << "Folder: " << getName() << endl;
        for (EIt it = e_tree.begin(); it != e_tree.end(); it++) {
            if (it->second->isFolder()) {
                Folder *fl = dynamic_cast<Folder *>(it->second);
                fl->dumpTree(level+1); // dump subtree
            }
            else {
                Bmark *bm = dynamic_cast<Bmark *>(it->second);
                cout << ms(level) << "Bmark: " << it->first << " = " << bm->getPayload() << endl;
            }
        }
    }

    void dumpTree(Folder * root, FILE * file, int level = 1) {
        cout << ms (level-1) << "Folder: " << getName() << endl;

//        fprintf(file, "Folder: %s\n", getName().c_str());

        Folder * rt = dynamic_cast<Folder*>(root->findEntityRoot(getName()));
        if(rt != NULL)
            fprintf(file, "Folder: %s %s\n"/*,(ms(level-1)).c_str()*/,/*level-1*/rt->getName().c_str(), getName().c_str());
        else
            fprintf(file, "Folder: %s\n", getName().c_str());

        for (EIt it = e_tree.begin(); it != e_tree.end(); it++) {
            if (it->second->isFolder()) {
                Folder *fl = dynamic_cast<Folder *>(it->second);
                fl->dumpTree(root, file, level+1); // dump subtree
            }
            else {
                Bmark *bm = dynamic_cast<Bmark *>(it->second);
                cout << ms(level) << "Bmark: " << it->first << " = " << bm->getPayload() << endl;
                Entity* parent = findEntityRoot((it->second)->getName());
//                fprintf(file, "Bmark: %s %s = %s\n",/*(ms(level)).c_str(),*/parent->getName().c_str(), (it->first).c_str(),(bm->getPayload()).c_str());
                fprintf(file, "Bmark: %s %s = %g %g %g %g %g %g %d\n",parent->getName().c_str(), (it->first).c_str(),bm->getX(),bm->getY(),bm->getZ(), bm->getPitch(),bm->getYaw(),bm->getRoll(),bm->getIdNavigation());
                //                qs.setValue(QString("Bmark"), QVariant(QString(it->first)));
//                qs->setValue(QString::fromStdString(it->first), QVariant(QString::fromStdString(bm->getPayload())));
//                sprintf(buf, "%sBmark: %s = %s\n",(ms(level)).c_str(),(it->first).c_str(),(bm->getPayload()).c_str());
            }
        }
    }

    Folder(string nm):Entity(nm) {

    }
};

bool is_prefix(const char *pre, const char *str);
Folder* readTree(const char *file);
//TODO:
void changeNameBmark(Folder * root, Bmark *bm, string oldname, string newname);
//void changeNameFolder(Folder * root, Folder *fld, string oldname, string newname);
void changeFolder(Folder * root, Folder * oldFolder, Folder * newFolder);

#endif // ENTITYBOOKMARK_H
