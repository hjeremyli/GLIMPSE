/*******************************************************************************
 * Copyright (C) 2020 Olivier Delaneau, University of Lausanne
 * Copyright (C) 2020 Simone Rubinacci, University of Lausanne
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "../../versions/versions.h"
#include <checker/checker_header.h>

void checker::declare_options() {
	bpo::options_description opt_base ("Basic options");
	opt_base.add_options()
			("help", "Produce help message")
			("seed", bpo::value<int>()->default_value(15052011), "Seed of the random number generator")
			("thread", bpo::value<int>()->default_value(1), "Number of threads");

	bpo::options_description opt_input ("Input files");
	opt_input.add_options()
			("input", bpo::value< string >(), "File listing True, Imputed, Frequencies and Regions")
			("samples", bpo::value< string >(), "List of samples to process, one sample ID per line")
			("info_af", bpo::value< string >()->default_value("AF"), "Name of the INFO/AF field to be used in the frequency file.");

	bpo::options_description opt_algo ("Parameters");
	opt_algo.add_options()
			("minPROB", bpo::value<double>(), "Minimum posterior probability P(G|R) in validation data")
			("minDP", bpo::value<int>(), "Minimum coverage in validation data")
			("bins", bpo::value< vector < double > >()->multitoken(), "Frequency bins used for Rsquare computations")
			("groups", bpo::value< string >(), "Alternative to frequency bins: bins are user defined");

	bpo::options_description opt_output ("Output files");
	opt_output.add_options()
			("output,O", bpo::value< string >(), "Report files")
			("log", bpo::value< string >(), "Log file");

	descriptions.add(opt_base).add(opt_input).add(opt_algo).add(opt_output);
}

void checker::parse_command_line(vector < string > & args) {
	try {
		bpo::store(bpo::command_line_parser(args).options(descriptions).run(), options);
		bpo::notify(options);
	} catch ( const boost::program_options::error& e ) { cerr << "Error parsing command line arguments: " << string(e.what()) << endl; exit(0); }

	if (options.count("log") && !vrb.open_log(options["log"].as < string > ()))
		vrb.error("Impossible to create log file [" + options["log"].as < string > () +"]");

	vrb.title("[GLIMPSE] Validate imputed data");
	vrb.bullet("Author        : Simone RUBINACCI & Olivier DELANEAU, University of Lausanne");
	vrb.bullet("Contact       : simone.rubinacci@unil.ch & olivier.delaneau@unil.ch");
	vrb.bullet("Version       : " + string(CONCORDANCE_VERSION));
	vrb.bullet("Run date      : " + tac.date());

	if (options.count("help")) { cout << descriptions << endl; exit(0); }
}

void checker::check_options() {
	if (!options.count("input"))
		vrb.error("You must specify --input");

	if (!options.count("output"))
		vrb.error("You must specify --output");

	if (!options.count("minPROB"))
		vrb.error("You must specify --minPROB");

	if (!options.count("minDP"))
		vrb.error("You must specify --minDP");

	if ((options.count("bins")+options.count("groups")) != 1)
		vrb.error("You must specify --bins or --groups");
}

void checker::verbose_files() {
	vrb.title("Files are listed in [" + options["input"].as < string > () + "]");
	if (options.count("log")) vrb.bullet("Output LOG    : [" + options["log"].as < string > () + "]");
	if (options.count("groups")) {
		vrb.bullet("Groups        : [" + options["groups"].as < string > () + "]");
	}
}

void checker::verbose_options() {
	vrb.title("Parameters:");
	vrb.bullet("Seed    : " + stb.str(options["seed"].as < int > ()));
	vrb.bullet("MinPROB : " + stb.str(options["minPROB"].as < double > ()));
	vrb.bullet("MinDP   : " + stb.str(options["minDP"].as < int > ()));
	vrb.bullet("#Threads   : " + stb.str(options["thread"].as < int > ()));
	if (options.count("bins")) {
		vector < double > tmp = options["bins"].as < vector < double > > ();
		vrb.bullet("#bins   : " + stb.str(tmp.size()));
	} else {
		vrb.bullet("#groups : User defined");
	}
}
