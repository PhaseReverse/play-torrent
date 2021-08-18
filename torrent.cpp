#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/file_storage.hpp>
#include <libtorrent/torrent_flags.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/settings_pack.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/download_priority.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

//#include <unistd.h>

int main(int argc, char* argv[])
{
    //DEFAULTS//
    std::string dir="."; //SAVE DIRECTORY
    std::string upRate = "0"; //UPLOAD RATE 0 MEANS UNLIMITED
    std::string downRate = "0"; //DOWNLOAD RATE 0 MEANS UNLIMITED
    std::string playerCmd = "mpv"; //MEDIA PLAYER COMMAND
    bool select = false;  //OPTION FOR FILES SELECTION
    bool exit = false; //EXIT AFTER FINISH DOWNLOADING
    if (argc < 2) {
        printf("error: no operation specified (use -h for help)\n");
        return 1;
    }


    //HELP MENU//
    if (strcmp (argv[1],"-h") == 0) {
        printf("usage: torrent [\"magnet-url\"/torrent-file] <operations>\n");
        printf("operations:\n");
        printf("\t-d [directory] Directory where torrent download will be saved\n");
        printf("\t-p [player command] Media Player to play media files\n");
        printf("\t-x Exit when downloading is finished\n");
        printf("\t-s Allow user to select which files not to download\n");
        printf("\t-up [upload speed in kbps] Upload speed limit\n");
        printf("\t-down [download speed in kbps] Download speed limit\n");

        return 0;

    }
    //COMMANDLINE ARGUMENTS PARSER//
    for(int i=2; i<argc; i++) {

        if(strcmp (argv[i],"-d") == 0) {
            i++;
            if(i<argc)
                dir = argv[i];
            continue;
        }
        if(strcmp (argv[i],"-up") == 0) {
            i++;
            if(i<argc)
                upRate = argv[i];
            continue;
        }
        if(strcmp (argv[i], "-down")==0) {
            i++;
            if(i<argc)
                downRate = argv[i];
            continue;

        }
        if(strcmp (argv[i], "-p")==0) {
            i++;
            if(i<argc)
                playerCmd = argv[i];
            continue;

        }
        if(strcmp (argv[i], "-s")==0) {

            select = true;
            continue;

        }
        if(strcmp (argv[i], "-x")==0) {

            exit = true;

        }

    }

    //CREATE SESSIONS AND ADD TORRENT//
    lt::settings_pack p;
    p.set_int(lt::settings_pack::alert_mask, lt::alert_category::status | lt::alert_category::error);
    lt::session ses(p);
    lt::add_torrent_params atp;
    std::string url = argv[1];
    if(url.rfind("magnet:?xt=urn:btih:",0)!=0) {
        lt::torrent_info tInfo(argv[1]);
        atp = lt::parse_magnet_uri(lt::make_magnet_uri(tInfo));
    }
    else {
        atp = lt::parse_magnet_uri(argv[1]);
    }
    atp.save_path = dir;
    lt::torrent_handle h = ses.add_torrent(atp);



    //GET METADATA AND SELECT FILES NOT TO DOWNLOAD//

    for (;;) {
        std::vector<lt::alert*> alerts;
        ses.pop_alerts(&alerts);
        for (lt::alert const* a : alerts) {
            std::cout << a->message() << std::endl;
            if (lt::alert_cast<lt::metadata_received_alert>(a)) {
                goto metadata_done;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

metadata_done:

    for(int i=0; i<h.torrent_file()->num_files(); i++)
        std::cout << "("<<i+1<<") " << h.torrent_file()->orig_files().file_path(i) << std::endl;

    std::cout << "Enter a selection with spaces eg:1 4 11 (For not downloading): ";
    std::string line;
    std::getline (std::cin,line);
    std::stringstream in(line);
    int iline;

    while(in >> iline) {
        h.file_priority(iline,0);  //Set Priority to 0 of selection to not download
    }


	//START DOWNLOADING
	




}
