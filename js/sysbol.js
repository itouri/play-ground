var iterable = {
    a:1,
    b:0
};

iterable[Symbol.iterator] = function(){
    //イテレータを返す
    return {
        iterable: this,
        next:function(){
            if(this.iterable.a>100){
                //無限ループ防止のため100を超えたら打ち切り
                return {
                done: true,
                value: null
                };
            }
            var n = this.iterable.a + this.iterable.b;
            var oldb = this.iterable.b;
            this.iterable.a = oldb;
            this.iterable.b = n;
            return {
                value: oldb,
                done: false
            };
        }
    };
};

//ためしに回してみる
for(var n of iterable){
console.log(n);
}