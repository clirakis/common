{
    unsigned  SecPerDay  = 60*60*24;
    unsigned  SecPerWeek = SecPerDay*7;
    // First Epoch is January 6, 1980. 
    time_t    gps_epoch1 = 315964800;
    // Second Epoch is August 22, 1999
    time_t    gps_epoch2 = 935280000;

    Double_t GPSWeek  = 70801.3;
    Double_t Extended = 844.0;
    Double_t Delta    = 17.0;
    struct timespec PCTime = {1445801984,555083860};
    (GPSWeek+Delta+gps_epoch2+844*SecPerWeek) - PCTime.tv_sec;
}
