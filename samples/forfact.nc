using lin;

var i = 0;
var n = 5;
var f = 1;

for (var n = 1; n < 7; n = n + 1;) {
    f = 1;
    for (var i = 1; i < n + 1; i = i + 1;) {
        f = f * i;
    };
    print f;
};
