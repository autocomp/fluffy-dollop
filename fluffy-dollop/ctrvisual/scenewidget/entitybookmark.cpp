#include "entitybookmark.h"

bool is_prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

Folder* readTree(const char *file) {
    return NULL;
/*
    FILE *fp = fopen(file, "r");
    char *line = NULL;
    size_t len;
    ssize_t rn, rs;
    char name[128], parent[128];

    std::cout<<"readTree"<<std::endl;

    rn = getline(&line, &len, fp); // read root definition
    rs = sscanf(line, "Folder: %s", name);

    const char * str = "Bookmarks";
    if(strcmp(name, str)!=0)
        return NULL;

    std::cout<<"line = "<<line<<" "<<"NAME = "<<name<<"NAME"<<std::endl;
    if (rs == -1) {
        cout << "Can't read root folder" << endl; // error
        return NULL;
    }

    Folder *root = new Folder(name);
    printf ("Created root with name %s\n", name);

    while (!feof(fp)) {
        line[0] = '\0';
        rn = getline(&line, &len, fp);

        if (is_prefix("Bmark:", line)) {
            float cam0,cam1,cam2,cam3,cam4,cam5;
            int id;
            int rs = sscanf(line, "Bmark: %s %s = %g %g %g %g %g %g %d", parent,
                            name, &cam0,&cam1,&cam2,&cam3,&cam4,&cam5,&id);

            Folder* bmParent = dynamic_cast<Folder *>(root->findEntity(parent));
            if (!bmParent) {
                printf ("Error parsing ``%s''...No such folder: %s\n", line, parent); // error
                return NULL;
            }
            Bmark * newbmark = new Bmark(name, cam0,cam1,cam2,cam3,cam4,cam5,id);
            newbmark->setAngle(cam3,cam4,cam5);
            newbmark->setOrigin(cam0,cam1,cam2);
            newbmark->setNavigation(id);
            bmParent->addEntity(newbmark);
            std::cout<<"set Angle: "<<cam3<<"    "<<cam4<<"  "<<cam5<<std::endl;
            std::cout<<"set Origin: "<<cam0<<"    "<<cam1<<"  "<<cam2<<std::endl;
            std::cout<<"set Navigation: "<<id<<std::endl;
//            bmParent->addEntity(new Bmark(name, cam0,cam1,cam2,cam3,cam4,cam5));
        }
        else if (is_prefix("Folder:", line)) {

            int rs = sscanf(line, "Folder: %s %s", parent, name);
            Folder *fParent =  dynamic_cast<Folder *>(root->findEntity(parent));
            if (!fParent) {
                if(root->getName() == parent)
                    fParent = root;
                    //root->addEntity(new Folder(name));
                else
                {
                    printf ("Error parsing ``%s''...No such folder: %s\n", line, parent); // error
                    return NULL;
                }
            }
            fParent->addEntity(new Folder(name));
        }
        else {
            printf ("Omitting: %s\n", line);
            continue;
        }
    }
    return root;
*/
}

//TODO:
void changeNameBmark(Folder * root, Bmark* bm,string oldname, string newname){
    double x = bm->getX();
    double y = bm->getY();
    double z = bm->getZ();
    float pitch = bm->getPitch();
    float yaw = bm->getYaw();
    float roll = bm->getRoll();
    int id = bm->getIdNavigation();
    Bmark * newbmark = new Bmark(newname, x,y,z,pitch,yaw,roll,id);
    newbmark->setAngle(pitch,yaw,roll);
    newbmark->setOrigin(x,y,z);
    newbmark->setNavigation(id);
//    Folder* bmParent = dynamic_cast<Folder *>(root->findEntity(parent));
    Folder * parent;
    parent = root->findEntityRoot(oldname);
    parent->addEntity(newbmark);
    parent->deleteEntity(oldname);
}

//void changeNameFolder(Folder * root, Folder *fld, string oldname, string newname){

//}

void changeFolder(Folder * root, Folder * oldFolder, Folder * newFolder){
//    newFolder->getTree() = e_tree;

//    newFolder->getTree() = oldFolder->getTree();
    newFolder = oldFolder;

    root->deleteEntity(oldFolder->getName());
}
