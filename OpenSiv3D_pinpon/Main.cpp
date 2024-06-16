# include <Siv3D.hpp> // Siv3D v0.6.14
#if 1
const double BALL_X = 400.;
const double BALL_Y = 300.;
const double BALL_R = 5.;
const Vec2 BALL_V0 = { 2.5, 2.5 };
const double BAR1_X = 20.;
const double BAR1_Y = 300.;
const double BAR2_X = 780.;
const double BAR2_Y = 300.;

class BALL;
class Bar_Computer;

/*抽象クラス
* 純粋仮想関数 refrect Draw moveを持つ
*/
class Object {
public :
	virtual ~Object() = 0;
	virtual bool refrect(Vec2& ball, Vec2& ball_v, int& rally) = 0;
	virtual void Draw() = 0;
	virtual void move(const BALL& ball) = 0;
private :

};

Object::~Object(){}

class BAR :public Object {
public:
	BAR(const Vec2 &bar_): bar(bar_) {}//　コンストラクタ Vec2でプレイヤー側のバーを初期化
	/*バーとの反射
	* ボールと速度、ラリー数を参照型で引数とする
	* 反射した場合true,しない場合falseを返す
	*/
	virtual bool refrect(Vec2& ball, Vec2& ball_v, int &rally) {
		Vec2 p = ball + ball_v;		//移動後のボール
		if (ball_v.x > 0) {
			//ボールが負の方向かつ移動後、前ともにバーと接触しない場合判定せずfalseを返す
			if (p.x < bar.x || ball.x >= bar.x)return false;
		}
		else {
			//ボールが正の方向かつ移動後、前ともにバーと接触しない場合判定せずfalseを返す
			if (p.x > bar.x || ball.x <= bar.x)return false;
		}
		double bar0 = bar.y - 50;		//バーの中心からバーの頭頂部を得て、そこからそれぞれ三分かつする
		double bar1 = bar0 + 33.5;
		double bar2 = bar0 + 66.5;
		double bar3 = bar0 + 100;
		double ball_vec = ball_v.y / ball_v.x;		//傾きを得る
		double ball_line = (bar.x - ball.x) * ball_vec + ball.y;		//ボールとの線分ｙを得る
		if (bar0 <= ball_line && ball_line <= bar1) {	//前半三分の一と衝突した場合
			if (ball_v.y > 0) {		//速度yが+(下方向)の場合
				ball_v = { -ball_v.x, -ball_v.y };	//上方向に移動するよう反射
			}
			else if (ball_v.y < 0) {	//速度がy-
				ball_v = { -(ball_v.x * sqrt(2)),0 }; //直線運動にする
			}
			else if (ball_v.y == 0) {	//ボールが直線運動の場合
				ball_v.x = -(ball_v.x / sqrt(2));	//上方向になるように反射
				ball_v.y = -ball_v.x;
			}
			ball = { bar.x, ball_line };	//ボールの衝突地点
			p -= ball;		//突き抜けた地点と衝突地点の線分の差分
			ball += p;		//ボールの位置を反射後にする
			if (rally % 10 == 0 && rally >= 10) {		//ラリー１０回ごとの判定
				if (ball_a > 1 && ball_v == BALL_V0)ball_a = 1;		//ボールが加速sれていないかつaが１より大きいときaを１にする
				ball_v = BALL_V0 * (1.0 + ball_a / 10);			//速度を初期速度から1.1,1.2となるように加速する
				ball_a++;
			}
			rally++;
			return true;
		}
		else if (bar1 < ball_line && ball_line <= bar2) {		//バーの中段部分
			ball_v = { -ball_v.x, ball_v.y };
			ball = { bar.x, ball_line };
			p -= ball;
			ball += p;
			if (rally % 10 == 0 && rally >= 10) {
				if (ball_a > 1 && ball_v == BALL_V0)ball_a = 1;
				ball_v = BALL_V0 * (1.0 + ball_a / 10);
				ball_a++;
			}
			rally++;
			return true;
		}
		else if (bar2 < ball_line && ball_line <= bar3) {		//バーの下段部分
			if (ball_v.y < 0) {
				ball_v = { -ball_v.x, -ball_v.y };
			}
			else if (ball_v.y > 0) {
				ball_v = { -(ball_v.x * sqrt(2)), 0 };
			}
			else if (ball_v.y == 0) {
				ball_v.x = -(ball_v.x / sqrt(2));
				ball_v.y = ball_v.x;
			}
			ball = { bar.x, ball_line };
			p -= ball;
			ball += p;
			if (rally % 10 == 0 && rally >= 10) {
				if (ball_a > 1 && ball_v == BALL_V0)ball_a = 1;
				ball_v = BALL_V0 * (1.0 + ball_a / 10);
				ball_a++;
			}
			rally++;
			return true;
		}
		return false;
	}
	virtual void Draw() {		//バーの描画
		RectF{ Arg::center(bar.x, bar.y), 5, 100 }.draw();
	}
	virtual void move(const BALL& ball) {		//キーの上下でバーを動かす
		if (KeyUp.pressed()) {
			bar.y -= bar_dy * Scene::DeltaTime();
			if (bar.y <= 5)bar.y = 5;
		}
		if (KeyDown.pressed()) {
			bar.y += bar_dy * Scene::DeltaTime();
			if (bar.y >= 595) bar.y = 595;
		}
	}
protected:
	double ball_a = 1;
	Vec2 bar;
private:
	double bar_dy = 200;

};

class border :public Object {
public:
	virtual bool refrect(Vec2& ball, Vec2& ball_v, int& rally) {
		if (ball_v.y == 0)return false;		//ボールが平行移動している場合falseを返す
		double ball_vec = ball_v.x / ball_v.y;
		Vec2 p = ball + ball_v;
		if (ball.y <= 5.0 && p.y <= 5.0) {	//上のボーダーラインを超えるかの判定
			double ball_line_upper = (5.0 - ball.y) * ball_vec + ball.x;	//ボーダーラインとの傾きy
			if (5.0 <= ball_line_upper && ball_line_upper <= 795) {		
				ball = { ball_line_upper, 5.0 };
				p -= ball;
				p.y = -p.y;
				ball += p;
				return true;
			}
		}
		if (ball.y >= 595 && p.y >= 595) {
			double ball_line_under = (595.0 - ball.y) * ball_vec + ball.x;
			if (5.0 <= ball_line_under && ball_line_under <= 795) {
				ball = { ball_line_under, 595.0 };
				p -= ball;
				p.y = -p.y;
				ball += p;
				return true;
			}
		}
		return false;
	}
	virtual void move(const BALL& ball) {}
	virtual void Draw() {

		const LineString line		//ボーダーラインを描く
		{
			Vec2{lineUX, lineUY}, Vec2{lineUX, lineDY},
			Vec2{lineUX, lineDY}, Vec2{lineDX, lineDY},
			Vec2{lineDX, lineUY}, Vec2{lineUX, lineUY}
		};
		line.draw();

	}

private:
	const double lineUX = 5, lineUY = 5, lineDX = 795, lineDY = 595;
};
#endif
#if 1
class BALL :public Object{
private:
	Vec2 ball, ball_v;

	int score1, score2;
public:
	BALL(const Vec2& ball_, const Vec2& ball_v_, int score1_, int score2_):ball(ball_), ball_v(ball_v_),
	score1(score1_), score2(score2_){}
	virtual void Draw() {
		Circle{ ball.x, ball.y, BALL_R }.draw();

	}
	void calc_ball(BAR& bar, border& line, Bar_Computer& bar2, int& rally);

	virtual void move(const BALL& ball_){}
	virtual bool refrect(Vec2& ball_, Vec2& ball_v_, int& rally) { return false; }
#if 1
	int Score1() {
		return score1;
	}
	int Score2() {
		return score2;
	}

	void reset_score() {
		score1 = 0, score2 = 0;
		ball = { BALL_X, BALL_Y };
		ball_v = BALL_V0;
	}

	const Vec2& get_ball() const{
		return ball;
	}
#endif
};
#endif
#if 1
class Bar_Computer :public BAR {

public:
	Bar_Computer(const Vec2& bar_):BAR(bar_){}
	virtual void move(const BALL& c_ball) {
		const Vec2 &ball = c_ball.get_ball();
		double dy = ball.y - bar.y;
		if (dy > 80)bar.y += 10.3;
		else if (dy > 50)bar.y += 6.3;
		else if (dy > 30)bar.y += 4.3;
		else if (dy > 10)bar.y += 3.3;
		else if (dy > -80)bar.y -= 10.3;
		else if (dy > -50)bar.y -= 6.3;
		else if (dy > -30)bar.y -= 4.3;
		else if (dy > -10)bar.y -= 3.3;
		if (bar.y >= 595.)bar.y = 595.;
		else if (bar.y <= 5.)bar.y = 5.;
	}
};
#endif
#if 1
void BALL::calc_ball(BAR& bar, border& line, Bar_Computer& bar2, int& rally) {

	bool colision = false;
#if 1
	colision = bar.refrect(ball, ball_v, rally);
	colision = bar2.refrect(ball, ball_v, rally);
#endif
#if 1
	if ((colision = line.refrect(ball, ball_v, rally)) == true) {
		ball_v.y = -ball_v.y;
	}
	if (colision == false)ball += ball_v;
#endif
	if (ball.x >= 795) {
		score1++;
		ball = { BALL_X, BALL_Y };
		ball_v = BALL_V0;
		rally = 0;
	}
	if (ball.x <= 5) {
		score2++;
		ball = { BALL_X, BALL_Y };
		ball_v = BALL_V0;
		rally = 0;
	}

	}
#endif
void Main()
{
	
#if 1
	Scene::SetBackground(Color( 0,0,0 ));
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
#endif
#if 1
	Vec2 ball( BALL_X, BALL_Y );
	Vec2 ball_v = BALL_V0;
#endif
#if 1
	String text1 = U"You Win";
	String text2 = U"You lose";
	String game_state = U"game";
	String restart = U"Restart Press 'R'";
	String quit = U"Quit Press 'Q'";

	const Font title{ 100 };
	const Font over_font{ 60 };
	const Font font{ 20 };
#endif

	BAR Bar(Vec2 (BAR1_X, BAR1_Y));
	BALL Ball(ball, ball_v, 0, 0);
	Bar_Computer cpu(Vec2 (BAR2_X, BAR2_Y));
	border line;
	int rally = 0;

	while (System::Update())
	{
#if 1
		if (game_state == U"game") {
			Ball.Draw();
			Bar.Draw();
			cpu.Draw();
			line.Draw();
			font(Ball.Score1()).draw(360, 20);
			font(Ball.Score2()).draw(440, 20);
			font(rally).draw(400, 40);
			Bar.move(Ball);
			cpu.move(Ball);
			Ball.calc_ball(Bar, line, cpu, rally);
			if (Ball.Score1() == 5) {
				game_state = U"over";
			}
			else if (Ball.Score2() == 5) {
				game_state = U"over";
			}
		}
#endif
#if 1
		else if (game_state == U"over") {
			if (Ball.Score1() == 5)title(text1).draw(Arg::center(400, 100));
			else title(text2).draw(Arg::center(400, 100));
			font(restart).draw(Arg::center(400, 300));
			font(quit).draw(Arg::center(400, 400));
			if (KeyR.down()) {
				Ball.reset_score();
				rally = 0;
				game_state = U"game";
			}
			else if (KeyQ.down()) {
				System::Exit();
			}

		}
#endif
	}

}
//
// - Debug ビルド: プログラムの最適化を減らす代わりに、エラーやクラッシュ時に詳細な情報を得られます。
//
// - Release ビルド: 最大限の最適化でビルドします。
//
// - [デバッグ] メニュー → [デバッグの開始] でプログラムを実行すると、[出力] ウィンドウに詳細なログが表示され、エラーの原因を探せます。
//
// - Visual Studio を更新した直後は、プログラムのリビルド（[ビルド]メニュー → [ソリューションのリビルド]）が必要な場合があります。
//
