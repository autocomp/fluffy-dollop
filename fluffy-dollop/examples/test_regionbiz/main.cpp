#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QPointF>
#include <QPolygonF>
#include <QWidget>

#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_entity_filter.h>
#include <regionbiz/rb_files_translator.h>

#include "test_reciver.h"
#include "ftp_checker.h"

void loadXlsx()
{
    using namespace regionbiz;
    auto mngr = RegionBizManager::instance();

    // select facility
    FacilityPtr facility = mngr->getBaseArea( 21 )->convert< Facility >();
    if( !facility )
    {
        qDebug() << "It's not facility";
        return;
    }

    auto childs = facility->getChilds();
    for( BaseAreaPtr child: childs )
    {
        qDebug() << "Start delete" << child->getName()
                 << ":" << mngr->deleteArea( child );
    }
    qDebug() << "All deleted";

    mngr->setCurrentEntity( facility->getId() );

    // load translator
    BaseDataTranslatorPtr ptr = BaseTranslator::convert< BaseDataTranslator >(
                mngr->getTranslatorByName( "xlsx" ));
    // set settings of path
    QVariantMap settings = {{ "file_path", "./data/гостиница Россия.xlsx" }};
    ptr->init( settings );
    QString err = "";
    bool try_check_all_load = ptr->checkTranslator( BaseDataTranslator::CT_READ, err );
    if( !try_check_all_load )
        qDebug() << "Yes, some wrong" << err;

    // load floors and rooms
    // after loading all areas in model system
    auto floors = ptr->loadFloors();
    auto rooms = ptr->loadRooms();
    qDebug() << "We load floors and rooms:" << floors.size() << rooms.size();

    // now we can modify and commit all that areas
    for( FloorPtr flo: floors )
    {
        qDebug() << "Start commit" << flo->getName()
                 << ":" << flo->commit();
    }
    for( RoomPtr roo: rooms )
    {
        qDebug() << "Start commit" << roo->getName()
                 << ":" << roo->commit();
    }
    qDebug() << "All commited";
}
void bigExample()
{
    using namespace regionbiz;

    //! init
    auto mngr = RegionBizManager::instance();
    QString str = "~/.contour_ng/regionbiz_psql.json";
    //QString str = "~/.contour_ng/regionbiz_sqlite.json";
    bool inited = mngr->init( str );

    //! load regons
    std::vector< RegionPtr > regions = mngr->getRegions();
    //! get first region
    RegionPtr reg = regions.front();
    //! print region info
    qDebug() << "Region info: " << reg->getDescription();
    for( QPointF coord: reg->getCoords() )
        qDebug() << "  Point reg: " << coord.x() << "x" << coord.y();

    //! get child locations (without facilitys)
    std::vector< BaseAreaPtr > locations = reg->getChilds( Region::RCF_LOCATIONS );
    //std::vector< LocationPtr > locations = mngr->getLocationsByParent( reg->getId() );

    //! print locations info
    for( BaseAreaPtr ptr: locations )
    {
        LocationPtr loc = BaseArea::convert< Location >( ptr );
        qDebug() << " New location: " << loc->getAddress() << ", " << loc->getDescription();

        for( QPointF coord: loc->getCoords() )
            qDebug() << "   Location Point: " << coord.x() << "x" << coord.y();
    }

    //! get child facilitys (with chllds of all child locations)
    std::vector< BaseAreaPtr > facilitys = reg->getChilds( Region::RCF_ALL_FACILITYS );
    // std::vector< FacilityPtr > facilitys = mngr->getFacilitysByParent( reg->getId() );
    // for( BaseLocationPtr loc: locations )
    // {
    //     auto facils = mngr->getFacilitysByParent( loc->getId() );
    //     for( FacilityPtr fac: facils )
    //         facilitys.push_back( fac );
    // }

    //! print facilitys info
    for( BaseAreaPtr fac_ptr: facilitys )
    {
        FacilityPtr fac = BaseArea::convert< Facility >( fac_ptr );
        qDebug() << " New Facility: " << fac->getDescription() << ", " << fac->getAddress() << ", "
                  << fac->getCadastralNumber();

        for( QPointF coord: fac->getCoords() )
            qDebug() << "  Facility Point: " << coord.x() << "x" << coord.y();
    }

    //! process floors
    for( BaseAreaPtr fac_ptr: facilitys )
    {
        //! load floors
        FacilityPtr fac = BaseArea::convert< Facility >( fac_ptr );
        FloorPtrs floors = fac->getChilds();
        // FlorsPtrs floors = mngr->getFloorsByParent( fac->getId() );

        for( FloorPtr flo: floors )
        {
            //! print info
            qDebug() << "  Floor info: " << flo->getNumber() << ": " << flo->getName() << ", "
                      << ( flo->getCoords().length() ? "have coords" : "don't have coords" );

            //! process rooms
            RoomPtrs rooms = flo->getChilds(  );
            for( BaseAreaPtr room_ptr: rooms )
            {
                RoomPtr room = BaseArea::convert< Room >( room_ptr );

                //! print info
                qDebug() << "    Room: " << room->getName() << ", "
                          << ( room->getCoords().length() ? "have coords" : "don't have coords" );
            }
        }
    }

    //! Selection test
    TestReciver recv;
    mngr->subscribeOnCurrentChange( &recv, SLOT(onSelection(uint64_t,uint64_t)) );
    mngr->setCurrentEntity( 4 );
    mngr->setCurrentEntity( 5 );

    mngr->subscribeCenterOn( &recv, SLOT(onCenterOn(uint64_t)) );
    mngr->centerOnEntity( 6 );

    //! test of metadata
    auto area = mngr->getBaseArea( 13 );
    // create some metadata
    area->addMetadata( "double", "area", 25 );
    BaseMetadataPtr data_type = MetadataFabric::createMetadata( "string", area->getId() );
    data_type->setName( "type" );
    data_type->setValueByVariant( "undefined" );
    area->addMetadata( data_type );
    // check present
    qDebug() << "Present \"type\":" << area->isMetadataPresent( "type" );
    // change val
    area->setMetadataValue( "type", "rent" );
    // get all meadata
    auto metadata = area->getMetadataMap();
    qDebug() << "Meta:" << metadata.size();
    for( auto data_pair: metadata )
    {
        BaseMetadataPtr data = data_pair.second;

        qDebug() << "  Data:" << data->getName() << "-" << data->getValueAsString();
        qDebug() << "  Variant val:" << data->getValueAsVariant();
    }
    BaseMetadataPtr data = area->getMetadata( "area" );
    qDebug() << " Data square:" << ( data ? data->getValueAsString() : "NONE" );
    // commit metadata (change base)
    // area->commit();

    //! test of entitys
    qDebug() << "Max id =" << BaseEntity::getMaxId();
    // create room
    auto room = mngr->addArea< Room >( 13 );
    qDebug() << "Max id after =" << BaseEntity::getMaxId();
    // change room
    room->setCoords(( QPolygonF() << QPointF( 1, 1 ) << QPointF( 1, 100 )
                      << QPointF( 100, 100 ) << QPointF( 100, 1 )));
    BaseArea::convert< Room >( room )->setName( QString::fromUtf8( "Тестовое имя" ));
    // commit room (change base)
    qDebug() << " Commit:" << room->commit();
    // test other geometry
    qDebug() << "  Geom:" << room->getCoords();
    room->setCoords( QPolygonF() << QPointF( 10, 10 ) << QPointF( 10, 1000 )
                     << QPointF( 1000, 1000 ) << QPointF( 1000, 10 )
                     << QPointF( 600, 50 ));
    qDebug() << " Commit again:" << room->commit();
    qDebug() << "  Geom:" << room->getCoords();

    //! check delete
    room->addMetadata( "double", "some", 10 );
    qDebug() << " Commit again:" << room->commit();
    qDebug() << " Delete area:" << mngr->deleteArea( room );
    qDebug() << "Max id after del =" << BaseEntity::getMaxId();

    //! check marks
    auto room_ptr = mngr->addArea< Room >( 13 );
    RoomPtr room_for_marks = BaseArea::convert< Room >( room_ptr );
    // add mark
    MarkPtr mark = room_for_marks->addMark( Mark::MT_DEFECT, QPointF( 30, 10 ));
    qDebug() << "Add mark:" << (bool) mark;
    MarkPtrs marks = room_for_marks->getMarks();
    if( marks.size() )
    {
        // add metadata
        MarkPtr mark = marks.at( 0 );
        qDebug() << "Add metadata to mark"
                 << mark->addMetadata( "string", "check", "Test" );
        // commit-delete
        qDebug() << "  Commit mark" << mark->commit();
        qDebug() << "  Delete mark" << mngr->deleteMark( mark );
    }
    // check again
    marks = room_for_marks->getMarks();
    qDebug() << "We have" << marks.size() << "marks after commit-delete";
    // check other variant
    MarkPtr mark_new = room_for_marks->addMark( Mark::MT_DEFECT, QPointF( 50, 20 ));
    mark_new->setName( "Тест" );
    mark_new->setDesription( "Описание" );

    marks = room_for_marks->getMarks();
    qDebug() << "We have" << marks.size() << "marks again";
    qDebug() << "  Commit room's marks:" << room_for_marks->commitMarks();
    qDebug() << "  Delete room's marks:" << room_for_marks->deleteMarks();
    marks = room_for_marks->getMarks();
    qDebug() << "We have" << marks.size() << "marks again after commit-delete";
    // clear
    mngr->deleteArea( room_ptr );
}

void selectManagment()
{
    using namespace regionbiz;

    //! init
    auto mngr = RegionBizManager::instance();

    // subscribes
    TestReciver rcv;
    mngr->subscribeOnCurrentChange( &rcv, SLOT( onCurrentChange( uint64_t, uint64_t )));
    mngr->subscribeOnSelectedSetChange( &rcv, SLOT( onSelectedSet( std::vector<uint64_t>, std::vector<uint64_t> )));

    // check select
    mngr->setCurrentEntity( 13 );
    // check repeat append
    mngr->appendToSelectedSet( 18 );
    mngr->appendToSelectedSet( { 19, 20 } );
    mngr->appendToSelectedSet( 18 );
    // and append again
    mngr->appendToSelectedSet( 18, false );
    // check clear
    mngr->clearSelect();

    // if select and clear. Select 0 and clear signals:
    mngr->setCurrentEntity( 14 );
    mngr->clearSelect();
}

void filterCheck()
{
    using namespace regionbiz;

    //! init
    auto mngr = RegionBizManager::instance();
    QString str = "contour_ng\\regionbiz_psql.json";
    bool inited = mngr->init( str );

    // make filter
    BaseFilterParamPtrs filters;

    auto area_filter = BaseFilterParamFabric::createFilter< DoubleFilterParam >( "area" );
    area_filter->setMax( 20 );
    area_filter->setMin( 10 );
    area_filter->setGetEmptyMetadate( false );
    filters.push_back( area_filter );

    auto status_filter = BaseFilterParamFabric::createFilter< StringFilterParam >( "status" );
    status_filter->setRegExp( QString::fromUtf8( "Свободно" ));
    //filters.push_back( status_filter );

    auto rentor_filter = BaseFilterParamFabric::createFilter< StringFilterParam >( "rentor" );
    rentor_filter->setRegExp( QString::fromUtf8( "*АО*" ));
    filters.push_back( rentor_filter );

    EntityFilter::setFilters( filters );

    // load all locations
    std::vector< RegionPtr > regions = mngr->getRegions();
    RegionPtr reg = regions.front();

    // metade empty check
    qDebug() << "Meta Data:" << reg->getMetadata( "test" )->getName() <<
                                reg->getMetadata( "test" )->getValueAsString();
    reg->getMetadata( "test" )->setValueByString( "some" );

    // print facilitys info
    std::vector< BaseAreaPtr > facilitys = reg->getChilds( Region::RCF_ALL_FACILITYS );
    for( BaseAreaPtr fac_ptr: facilitys )
    {
        FacilityPtr fac = BaseArea::convert< Facility >( fac_ptr );
        qDebug() << " Facility: " << fac->getDescription();
    }

    // process floors
    for( BaseAreaPtr fac_ptr: facilitys )
    {
        // load floors
        FacilityPtr fac = BaseArea::convert< Facility >( fac_ptr );
        FloorPtrs floors = fac->getChilds();

        for( FloorPtr flo: floors )
        {
            // print info
            qDebug() << "  Floor info: " << flo->getNumber() << " - " << flo->getName();

            // process rooms
            RoomPtrs rooms = flo->getChilds();
            for( BaseAreaPtr room_ptr: rooms )
            {
                RoomPtr room = BaseArea::convert< Room >( room_ptr );

                // print info
                qDebug() << "    Room: " << room->getName();
                QString data_info = "     Data:";
                if( room->isMetadataPresent( "area" ))
                    data_info += " Area: " + room->getMetadataValue( "area" ).toString();
                if( room->isMetadataPresent( "status" ))
                    data_info += " Status: " + room->getMetadataValue( "status" ).toString();
                if( room->isMetadataPresent( "rentor" ))
                    data_info += " Rentor: " + room->getMetadataValue( "rentor" ).toString();
                qDebug() << "" << data_info;
            }
        }
    }
}

void checkCommitAndDelete()
{
    using namespace regionbiz;

    //! init
    auto mngr = RegionBizManager::instance();
    QString str = "contour_ng\\regionbiz_psql.json";
    bool inited = mngr->init( str );

    // get region
    std::vector< RegionPtr > regions = mngr->getRegions();
    RegionPtr reg = regions.front();

    // make area
    auto loc = mngr->addArea< Location >( reg );
    loc->addMetadata( "double", "test_field", 12 );
    loc->setCoords( QPolygonF() << QPointF( 10, 20 ) <<  QPointF( 15, 16 ));
    loc->setName( QString::fromUtf8( "Тестовая локация" ));

    QTime time;

    time.restart();
    bool comm = loc->commit();
    std::cout << "Location " << loc->getId() << " was commited: " << comm << time.elapsed() << std::endl;

    // create and commit childs
    time.restart();
    bool comm_ch = true;
    for( int i = 0; i < 100; ++i )
    {
        auto fac = mngr->addArea< Facility >( loc );
        fac->addMetadata( "double", "test_field", 12 );
        fac->addMetadata( "double", "test_field2", 25 );
        fac->setCoords( QPolygonF() << QPointF( 10, 20 ) <<  QPointF( 15, 16 )
                        << QPointF( 100, 200 ) <<  QPointF( 150, 160 ));
        fac->setName( QString::fromUtf8( "Тестовое здание" ));
        comm_ch = comm_ch && fac->commit();
    }
    qDebug() << "Child commited:" << comm_ch << time.elapsed();

    time.restart();
    bool del = mngr->deleteArea( loc );
    std::cout << "Location " << loc->getId() << " was deleted: " << del << time.elapsed() << std::endl;
}

void init()
{
    using namespace regionbiz;

    //! init
    auto mngr = RegionBizManager::instance();
    QString str = "config/regionbiz.json";
    bool inited = mngr->init( str );
}

void checkFtp()
{
    FtpChecker* ftp_check = new FtpChecker();
}

void checkMarksCoords()
{
    using namespace regionbiz;

    auto mngr = RegionBizManager::instance();
    std::vector< RegionPtr > regions = mngr->getRegions();
    RegionPtr reg = regions.front();
    auto locs = reg->getChilds( Region::RCF_LOCATIONS );
    LocationPtr location = locs[ 0 ]->convert< Location >();
    auto mark = location->addMark( Mark::MT_DEFECT );
    mark->setCoords( QPolygonF() << QPointF( 10, 10 ) << QPointF( 20, 20 ) << QPointF( 30, 15 ));
    qDebug () << "Commit mark:" << mark->commit();
    qDebug () << "Delete mark:" << mngr->deleteMark( mark );
}

void addFloors()
{
    using namespace regionbiz;

    auto mngr = RegionBizManager::instance();
    auto facil = mngr->getBaseArea( 5023 )->convert< Facility >();
    auto mark = facil->addMark( Mark::MT_PHOTO );
    mark->addMetadata( "double", "test" );
    qDebug() << "Commit:" << mark->commit();
    qDebug() << "Delete:" << mngr->deleteMark( mark );
}

void checkGroups()
{
    using namespace regionbiz;

    auto mngr = RegionBizManager::instance();
    // create
    auto group = mngr->addGroup();
    qDebug() << "Size of group" << group->getElements().size();

    // add elements
    auto floor = mngr->getBaseArea( 5 )->convert< Floor >();
    for( uint i = 0; i < 3; ++i )
        qDebug() << "Add" << floor->getChilds()[ i ]->moveToGroup( group );
    qDebug() << "Size of group" << group->getCount();

    // remove one element
    qDebug() << "Leave" << floor->getChilds()[ 0 ]->leaveGroup();
    qDebug() << "Size of group" << group->getCount();

    // double add element
    qDebug() << "Add" << floor->getChilds()[ 1 ]->moveToGroup( group );
    qDebug() << "Size of group" << group->getCount();

    // check commit
    qDebug() << "Commit" << group->commit();

    // check delete
    qDebug() << "Delete" << mngr->deleteGroup( group );

    // try to do somth with deleted group
    qDebug() << "Add" << floor->getChilds()[ 1 ]->moveToGroup( group );
    qDebug() << "Size of group" << group->getCount();
}

int main( int argc, char** argv )
{
    QApplication app( argc, argv );

    //! old exmple
    // bigExample();

    //! load xlsx
    // loadXlsx();

    //! filter checker
    // filterCheck();

    //! check commit and delete
    //checkCommitAndDelete();

    //! init
    init();

    //! select managment
    //selectManagment();

    //! check ftp
    checkFtp();

    //! check marks coords
    // checkMarksCoords();

    //! add floors
    // addFloors();

    //! check group working
    //checkGroups();

    // for check close program
    QWidget* wgt = new QWidget();
    wgt->show();

    return app.exec();
}
