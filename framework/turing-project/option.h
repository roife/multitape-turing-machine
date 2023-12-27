#pragma once

struct Option {
    bool verbose = false;
    bool help = false;
    char *tm_filename = nullptr;
    char *input = nullptr;

    option long_options[2] = {
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
    };

    Option(int argc, char *argv[]) {
        for (int ch; (ch = getopt_long_only(argc, argv, "vh", long_options, NULL)) != -1;) {
            switch (ch) {
                case 'v':
                    verbose = true;
                    break;
                case 'h':
                    help = true;
                    break;
                default:
                    std::cerr << "error: unknown option" << std::endl;
                    exit(ErrorCode::UNKNOWN_OPTION);
            }
        }

        if (optind < argc) {
            tm_filename = argv[optind++];
        }

        if (optind < argc) {
            input = argv[optind++];
        }
    }
};
