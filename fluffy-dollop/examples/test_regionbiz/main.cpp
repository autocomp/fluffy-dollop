#include <iostream>

#include <regionbiz/rb_manager.h>

int main()
{
    using namespace regionbiz;

    Location* loc = new Location(1);
    delete loc;

    //! init
    auto mngr = RegionBizManager::instance();
    std::string str = "./config.json";
    mngr->init( str );

    //! load regons
    std::vector< RegionPtr > regions = mngr->getRegions();
    //! get first region
    RegionPtr reg = regions.front();
    //! print region info
    std::cout << "Region info: " << reg->getDescription() << std::endl;
    for( Coord coord: reg->getCoords() )
        std::cout << "  Point reg: " << coord.x << "x" << coord.y << std::endl;

    //! get child locations (without facilitys)
    std::vector< BaseAreaPtr > locations = reg->getChilds( Region::RCF_LOCATIONS );
    //std::vector< LocationPtr > locations = mngr->getLocationsByParent( reg->getId() );

    //! print locations info
    for( BaseAreaPtr ptr: locations )
    {
        LocationPtr loc = BaseArea::convert< Location >( ptr );
        std::cout << " New location: " << loc->getAddress() << ", " << loc->getDescription()
                  << " (" << loc->getPlanPath() << ")" << std::endl;

        PlanKeeper::PlanParams params = loc->getPlanParams();
        std::cout << "  Params of plan: " << params.scale_h << "x" << params.scale_w
                  << ", " << params.rotate << ", " << params.x << "x" << params.y << std::endl;

        for( Coord coord: loc->getCoords() )
            std::cout << "   Location Point: " << coord.x << "x" << coord.y << std::endl;
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
        std::cout << " New Facility: " << fac->getDescription() << ", " << fac->getAddress() << ", "
                  << fac->getCadastralNumber() << std::endl;

        for( Coord coord: fac->getCoords() )
            std::cout << "  Facility Point: " << coord.x << "x" << coord.y << std::endl;
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
            std::cout << "  Floor info: " << flo->getNumber() << ": " << flo->getName() << ", "
                      << ( flo->getCoords().size() ? "have coords" : "don't have coords" )<< std::endl;

            //! process rooms
            BaseAreaPtrs rooms = flo->getChilds( Floor::FCF_ALL_ROOMS );
            for( BaseAreaPtr room_ptr: rooms )
            {
                RoomPtr room = BaseArea::convert< Room >( room_ptr );

                //! print info
                std::cout << "    Room: " << room->getName() << ", "
                          << ( room->getCoords().size() ? "have coords" : "don't have coords" )<< std::endl;
            }
        }
    }
}
