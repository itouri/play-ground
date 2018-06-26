import { MatIV } from './minMatrix';
import { WebGL } from './webgl';


//TODO 型anyは良くない
function createMatrix() : any {
    let m = new MatIV();

    // 各種行列の生成と初期化
    let mMatrix = m.identity(m.create());
    let vMatrix = m.identity(m.create());
    let pMatrix = m.identity(m.create());
    let mvpMatrix = m.identity(m.create());
    
    // ビュー座標変換行列
    m.lookAt([0.0, 1.0, 3.0], [0, 0, 0], [0, 1, 0], vMatrix);
    
    // プロジェクション座標変換行列
    //TODO width, heightのハードコーディング
    const width = 300;
    const height = 300;
    m.perspective(90, width / height, 0.1, 100, pMatrix);
    
    // 各行列を掛け合わせ座標変換行列を完成させる
    m.multiply(pMatrix, vMatrix, mvpMatrix);
    m.multiply(mvpMatrix, mMatrix, mvpMatrix);

    return mvpMatrix;
}

function main() : void {
    const canvas = <HTMLCanvasElement>document.getElementById('canvas');
    let context = canvas.getContext('webgl')!;

    let webgl = new WebGL(context);
    webgl.gl.clearColor(0.0, 0.0, 0.0, 1.0);
    webgl.gl.clearDepth(1.0);
    webgl.gl.clear( webgl.gl.COLOR_BUFFER_BIT | webgl.gl.DEPTH_BUFFER_BIT );

    const vertexCode = `
        attribute vec3 position;
        uniform   mat4 mvpMatrix;

        void main() {
            gl_Position = mvpMatrix * vec4(position, 1.0);
        }
    `;

    const fragmentCode = `
        void main(void){
            gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    `;

    const program = webgl.createProgram(vertexCode, fragmentCode);

    const vertices = [
        0.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0
    ];
    const vertexBuffer = [
        webgl.createVertexBuffer(vertices),
    ];
   
    //TODO なんかそうじゃない
    let attLocations = [
        webgl.gl.getAttribLocation(program, 'position'),
    ];
    let attStrides = [3];

    //wgldではcreateProgram()の時点で行っている
    webgl.gl.useProgram(program);

    webgl.setAttribute(vertexBuffer, attLocations, attStrides)

    const uniLocation = webgl.gl.getUniformLocation(program, 'mvpMatrix');
    const mvpMatrix = createMatrix();
    webgl.gl.uniformMatrix4fv(uniLocation, false, mvpMatrix);

    webgl.gl.clearColor(0.3, 0.3, 0.3, 1);
    webgl.gl.clear(webgl.gl.COLOR_BUFFER_BIT);
    webgl.gl.drawArrays(webgl.gl.TRIANGLES, 0, 3);
}

onload = function() {
    main();
}