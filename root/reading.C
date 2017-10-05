void reading () { 
  ifstream in ("pippo.txt") ;
  double x = -1 ;
  double y = -1 ;
  double z = -1 ;
  while (!in.eof()) {
        in >> x >> y >> z ;
        if (!in.good()) break;
        std::cout << "fino a qui ok " 
                  << x << "\t" 
                  << y << "\t" 
                  << z << "\t" 
                  << std::endl ;
  }
}