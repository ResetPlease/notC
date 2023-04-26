using lin;

var n = 5;
var f = 1;

while (n>1;) {
    f = f*n;
    n = n-1;
    if (n>1;) {
        print n;
    };
    if (n>3;) {
        print "MORE THAN 3";
        endif
    } else {
        print "LESS THAN 3";
    };
};

print f;
