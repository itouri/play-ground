import { MatIV } from './minMatrix';

function compileShader(gl : WebGLRenderingContext, shader : WebGLShader, code : string) : void {
    gl.shaderSource(shader, code);
    gl.compileShader(shader);

    if(!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.log(gl.getShaderInfoLog(shader));
        throw new Error('compile error');
    }
}

function createProgram(gl : WebGLRenderingContext, vertexCode : string, fragmentCode : string) : WebGLProgram {
    const vertexShader = gl.createShader(gl.VERTEX_SHADER)!;
    compileShader(gl, vertexShader, vertexCode);

    const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER)!;
    compileShader(gl, fragmentShader, fragmentCode);

    const program = gl.createProgram()!;
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);

    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        console.log(gl.getProgramInfoLog(program));
        throw new Error("program error");
    }
    return program;
}

function createVertexBuffer(gl : WebGLRenderingContext, vertices : number[]) : WebGLBuffer {
    const buffer = gl.createBuffer()!;

    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, null); //TODO いらないの?

    return buffer;
}

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
    m.perspective(90, 300 / 300, 0.1, 100, pMatrix);
    
    // 各行列を掛け合わせ座標変換行列を完成させる
    m.multiply(pMatrix, vMatrix, mvpMatrix);
    m.multiply(mvpMatrix, mMatrix, mvpMatrix);

    return mvpMatrix;
}

function setAttribute(gl : WebGLRenderingContext, buffers : WebGLBuffer[], attLocations : number[], attStrides : number[] ) {
    for(let i in buffers){
        gl.bindBuffer(gl.ARRAY_BUFFER, buffers[i]);        
        gl.enableVertexAttribArray(attLocations[i]);
        gl.vertexAttribPointer(attLocations[i], attStrides[i], gl.FLOAT, false, 0, 0);
    }
}

function main() : void {
    const canvas = <HTMLCanvasElement>document.getElementById('canvas');

    let gl : WebGLRenderingContext;
    gl = canvas.getContext('webgl')!;
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.clearDepth(1.0);
    gl.clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );

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

    const program = createProgram(gl, vertexCode, fragmentCode);

    const vertices = [
        0.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0
    ];
    const vertexBuffer = [
        createVertexBuffer(gl, vertices),
    ];
   
    //TODO なんかそうじゃない
    let attLocations = [
        gl.getAttribLocation(program, 'position'),
    ];
    let attStrides = [3];

    //wgldではcreateProgram()の時点で行っている
    gl.useProgram(program);

    setAttribute(gl, vertexBuffer, attLocations, attStrides)

    const uniLocation = gl.getUniformLocation(program, 'mvpMatrix');
    const mvpMatrix = createMatrix();
    gl.uniformMatrix4fv(uniLocation, false, mvpMatrix);

    gl.clearColor(0.3, 0.3, 0.3, 1);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.drawArrays(gl.TRIANGLES, 0, 3);
}

onload = function() {
    main();
}