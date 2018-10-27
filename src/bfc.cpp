#include <iostream>
#include <vector>
#include <getopt.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#define EOC '$'

namespace fs = std::filesystem;

namespace bfc
{
    std::string instruction_str;
    std::vector<std::size_t> instruction_stack;
    std::vector<unsigned char> byte_vector;
    std::size_t instruction_ptr;
    std::size_t byte_ptr;
    const int vector_size = 9999;
    std::string prg_name;
    enum error_codes
    {
        success,
        file_error,
        gen_error
    };
    void print_usage(void)
    {
        std::cout << "Usage\n" \
            << "\t-h        :\thelp\n" \
            << "\t-f <file> :\tread from file\n";
    }
    void init(char progname[])
    {
        instruction_ptr = 0;
        byte_ptr = 0;
        byte_vector.resize(vector_size);
        for ( auto i = 0;i < vector_size;i++)
            byte_vector.at(i) = 0;
        instruction_stack.reserve(50);
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
            while ( instruction_str.at(instruction_ptr) != EOC )
            {
                auto c = instruction_str.at(instruction_ptr);
                switch (c)
                {
                    case '+':
                        ++byte_vector.at(byte_ptr);
                        ++instruction_ptr;
                        break;
                    case '-':
                        if ( --byte_vector.at(byte_ptr) <= 0 )
                            byte_vector.at(byte_ptr) = 0;
                        ++instruction_ptr;
                        break;
                    case '>':
                        ++byte_ptr;
                        ++instruction_ptr;
                        break;
                    case '<':
                        if ( --byte_ptr <= 0 )
                            byte_ptr = 0;
                        ++instruction_ptr;
                        break;
                    case '.':
                        std::cout << byte_vector.at(byte_ptr);
                        ++instruction_ptr;
                        break;
                    case ',':
                        byte_vector.at(byte_ptr) = std::cin.get();
                        ++instruction_ptr;
                        break;
                    case '[':
                        instruction_stack.push_back(instruction_ptr);
                        if (  byte_vector.at(byte_ptr) != 0 )
                            ++instruction_ptr;
                        else
                            instruction_ptr = instruction_str.find(instruction_ptr,']');
                        break;
                    case ']':
                        if (  byte_vector.at(byte_ptr) == 0 )
                            ++instruction_ptr;
                        else
                        {
                            instruction_ptr = instruction_stack.back();
                            instruction_stack.pop_back();
                        }
                        break;
                    default:
                        ++instruction_ptr;
                }
            }
            return;
        }

        int error_check(void)
        {
            int l_count = 0,r_count = 0;
            for ( auto j : instruction_str )
            {
                if ( j == '[' )
                    ++l_count;
                else if ( j == ']' )
                    ++r_count;
            }
            if ( l_count != r_count )
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
    bfc::init(argv[0]);
    if ( argc == 1 )
    {
        /* if no args
         * read input from stdin
         */
        while ( std::getline(std::cin,line) )
            buffer += line;
    }
    else
    {
        while ( (option = getopt(argc,argv,"hf:") ) != -1 )
        {
            switch (option)
            {
                case 'h':
                    bfc::print_usage();
                    break;
                case 'f':
                    filename.assign(optarg);
                    if ( ! fs::exists(filename) )
                    {
                        std::cerr << argv[0] << " : File not found - " << filename << std::endl;
                        return bfc::file_error;
                    }
                    else
                    {
                        ifile.open(filename);
                        while ( std::getline(ifile,line) )
                            buffer += line;
                    }
                    break;
                default:
                    bfc::print_usage();
                    break;
            }
        }
        for ( auto i = optind;i < argc;i++)
            std::cerr << argv[0] << " : Unrecognized option " << argv[i] << std::endl;
        if ( optind < argc )
        {
            bfc::print_usage();
            return bfc::gen_error;
        }
    }
    buffer += EOC;
    bfc::set_input_string(buffer);
    if ( bfc::parser::error_check() == bfc::parser::bracket_error )
    {
        std::cerr << argv[0] << ": Syntax error '['/']' " << std::endl;
        return bfc::parser::bracket_error;
    }
    bfc::parser::parse();

    return 0;
}
