#include <iostream>
#include <vector>
#include <getopt.h>
#include <unistd.h>
#include <fstream>

/*
 * EOC is a symbol appended
 * to the end of the code buffer
 */

#define EOC '$'

/*
 * alias std::filesystem for fewer keystrokes
 */
#ifndef __cpp_lib_filesystem
    #include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
    #include <filesystem>
    namespace fs = std::filesystem;
#endif

/*
 * bfc namespace to hold soms global
 * variables and functions
 */
namespace bfc
{
    std::string instruction_str;
    std::vector<unsigned char> byte_vector;
    std::size_t instruction_ptr;
    std::size_t byte_ptr;
    const int vector_size = 9999;
    std::string prg_name;
    int match_count;
    enum error_codes
    {
        success,
        file_error,
        gen_error
    };
    void print_usage(void)
    {
        std::cout << "bfc - A simple Brainfuck Interpreter" << std::endl \
        << "Usage" << std::endl \
        << "\t-h        :\thelp" << std::endl \
        << "\t-f <file> :\tread from file" << std::endl \
        << "\t-c <code> :\tfeed code directly as input\n";
    }

    /*
     * init function to initialize byte vector
     * byte pointer,instruction pointer
     */
    void init(char progname[])
    {
        instruction_ptr = 0;
        byte_ptr = 0;
        byte_vector.resize(vector_size);
        for ( auto i = 0;i < vector_size;i++)
            byte_vector.at(i) = 0;
        prg_name.assign(progname);
        return;
    }
    void set_input_string(std::string str)
    {
        instruction_str.assign(str);
        return;
    }
    namespace parser
    {
        enum p_error_code
        {
            bracket_error,
            p_success
        };
        void parse(void)
        {
            for( ;instruction_str.at(instruction_ptr) != EOC; instruction_ptr++ )
            {
                auto c = instruction_str.at(instruction_ptr);
                switch (c)
                {
                    case '+':
                        ++byte_vector.at(byte_ptr);
                        break;
                    case '-':
                        --byte_vector.at(byte_ptr);
                        break;
                    case '>':
                        ++byte_ptr;
                        break;
                    case '<':
                        if ( --byte_ptr <= 0 )
                            byte_ptr = 0;
                        break;
                    case '.':
                        std::cout << byte_vector.at(byte_ptr);
                        break;
                    case ',':
                        byte_vector.at(byte_ptr) = std::cin.get();
                        break;
                    case '[':
                        if ( byte_vector.at(byte_ptr) == 0 )
                        {
                            int bracket_c = 1;
                            while ( bracket_c )
                            {
                                c = instruction_str.at(++instruction_ptr);
                                if ( c == ']' )
                                    --bracket_c;
                                else if ( c == '[' )
                                    ++bracket_c;
                            }
                        }
                        break;
                    case ']':
                        if ( byte_vector.at(byte_ptr) != 0 )
                        {
                            int bracket_c = 1;
                            while ( bracket_c > 0 )
                            {
                                c = instruction_str.at(--instruction_ptr);
                                if ( c == '[' )
                                    --bracket_c;
                                else if ( c == ']' )
                                    ++bracket_c;
                            }
                        }
                        break;
                }
            }
            return;
        }

        int error_check(void)
        {
            int l_count = 0;
            for ( auto j : instruction_str )
            {
                if ( j == '[' )
                    ++l_count;
                else if ( j == ']' )
                    --l_count;
            }
            if ( l_count )
                return bracket_error;
            else
                return p_success;
        }
    };
};

int main(int argc,char **argv)
{
    std::string filename;
    std::string buffer,line;
    int option;
    std::ifstream ifile;
    bool opt_flag = false;
    if ( argc == 1 )
    {
        /* if no args
         * read input from stdin
         */
        bfc::init(argv[0]);
        while ( std::getline(std::cin,line) )
            buffer += line;
    }
    else
    {
        /*
         * parse command line arguments
         */
        while ( (option = getopt(argc,argv,"hf:c:") ) != -1 )
        {

            if ( option == 'h' )
            {
                    bfc::print_usage();
                    opt_flag = true;
                    break;
            }
            else if ( option == 'f' )
            {
                filename.assign(optarg);
                if ( ! fs::exists(filename) )
                {
                    std::cerr << argv[0] << " : File not found - " << filename << std::endl;
                    return bfc::file_error;
                }
                else
                {
                    bfc::init(argv[0]);
                    ifile.open(filename);
                    while ( std::getline(ifile,line) )
                        buffer += line;
                }
                break;
            }
            else if ( option == 'c' )
            {
                buffer.assign(optarg);
                bfc::init(argv[0]);
                break;
            }
            else
            {
                bfc::print_usage();
                opt_flag = true;
                break;
            }
        }
        if ( option == '?' )
            opt_flag = true;
        if ( optind < argc  &&  (! opt_flag ) )
        {
            std::cerr << argv[0] << " : Unrecognized option" << std::endl;
            bfc::print_usage();
            return bfc::gen_error;
        }
    }
    buffer += EOC;
    bfc::set_input_string(buffer);
    if ( bfc::parser::error_check() == bfc::parser::bracket_error )
    {
        std::cerr << argv[0] << ": Error - Mismatch brackets " << std::endl;
        return bfc::parser::bracket_error;
    }
    bfc::parser::parse();
    return 0;
}
