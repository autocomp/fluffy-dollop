#include <QDebug>
#include <QDir>

#include <regionbiz/rb_manager.h>

#include "test_reciver.h"

void loadXlsx()
{
    using namespace regionbiz;
    auto mngr = RegionBizManager::instance();

    // select facility
    BaseAreaPtr facility = mngr->getBaseArea( 21 )->convert< Facility >();
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

    mngr->selectEntity( facility->getId() );

    // load translator
    BaseTranslatorPtr ptr = mngr->getTranslatorByName( "xlsx" );
    // set settings of path
    QVariantMap settings = {{ "file_path", "./data/гостиница Россия.xlsx" }};
    ptr->init( settings );
    QString err = "";
    bool try_check_all_load = ptr->checkTranslator( BaseTranslator::CT_READ, err );
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
        qDebug() << " New location: " << loc->getAddress() << ", " << loc->getDescription()
                  << " (" << loc->getPlanPath() << ")";

        PlanKeeper::PlanParams params = loc->getPlanParams();
        qDebug() << "  Params of plan: " << params.scale_h << "x" << params.scale_w
                  << ", " << params.rotate << ", " << params.x << "x" << params.y;

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
            BaseAreaPtrs rooms = flo->getChilds( Floor::FCF_ALL_ROOMS );
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
    mngr->subscribeOnSelect( &recv, SLOT(onSelection(uint64_t,uint64_t)) );
    mngr->selectEntity( 4 );
    mngr->selectEntity( 5 );

    mngr->subscribeCenterOn( &recv, SLOT(onCenterOn(uint64_t)) );
    mngr->centerOnEntity( 6 );

    //! test of metadate
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
    // commit metadate (change base)
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
    MarkPtr mark = room_for_marks->addMark( QPointF( 30, 10 ));
    qDebug() << "Add mark:" << (bool) mark;
    MarkPtrs marks = room_for_marks->getMarks();
    if( marks.size() )
    {
        // add metadata
        MarkPtr mark = marks.at( 0 );
        qDebug() << "Add metadate to mark"
                 << mark->addMetadata( "string", "check", "Test" );
        // commit-delete
        qDebug() << "  Commit mark" << mark->commit();
        qDebug() << "  Delete mark" << mngr->deleteMark( mark );
    }
    // check again
    marks = room_for_marks->getMarks();
    qDebug() << "We have" << marks.size() << "marks after commit-delete";
    // check other variant
    MarkPtr mark_new = room_for_marks->addMark( QPointF( 50, 20 ));
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
    QString str = "contour_ng\\regionbiz_sqlite.json";
    bool inited = mngr->init( str );

    // subscribes
    TestReciver rcv;
    mngr->subscribeOnSelect( &rcv, SLOT( onSelection(uint64_t ,uint64_t )) );
    mngr->subscribeClearSelect( &rcv, SLOT( onClearSelect() ));
    mngr->subscribeSelectedSet( &rcv, SLOT( onSelectedSet(std::set<uint64_t> )));

    // check select
    mngr->selectEntity( 13 );
    // check repeat append
    mngr->appendToSelectedSet( 18 );
    mngr->appendToSelectedSet( 18 );
    mngr->appendToSelectedSet( 18 );
    // and append again
    mngr->appendToSelectedSet( 18 );
    // check clear
    mngr->clearSelect();

    // if select and clear. Select 0 and clear signals:
    mngr->selectEntity( 13 );
    mngr->clearSelect();
}

int main()
{
    //! old exmple
    // bigExample();

    //! load xlsx
    // loadXlsx();

    //! select managment
    selectManagment();
}
