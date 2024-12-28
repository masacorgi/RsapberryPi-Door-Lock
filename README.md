# Smart-Door-Lock
   
2021년 한국공학대학교 컴퓨터공학과 임베디드시스템 교과 과제 프로젝트를 기록한 공간입니다.   

Achro-em 라즈베리파이 개발보드를 활용한 간단한 스마트 도어락 시스템입니다.

* 목차
  1. 개발 목표
  2. 프로그램 흐름도
  3. 기능별 세부 설계
  4. 부록(소스코드)

<br>

## 1. 개발 목표


과제의 목표는 임베디드 리눅스 환경에서 수업 중 배운 RaspberryPi , 디바이스 드라이버를 최대한 많이 사용하여 동작하는 응용 소프트웨어를 개발하는 것이다.   
일상 생활에서 자주 접할 수 있는 현관문 도어락에서는 비밀번호 입력을 위해 여러 버튼이 필요하고(push switch)   
간단한 텍스트 출력이 가능하며(text LCD), 모터 동작(step moter)이 필요하므로 이번 과제물에 적합한 주제라고 판단할 수 있다.

<br>

## 2. 프로그램 흐름도
![image](https://github.com/user-attachments/assets/772e7b30-21a7-42f7-873c-d6b2e4f5d2e3)
<br>
1. 프로그램 실행 후 사용자는 비밀번호를 입력하고 싶다면 dip switch의 1번 스위치를 당긴다.
2. 간단한 환영 메시지가 출력(text lcd)되고, 사용자는 비밀번호 네자리를 입력한다.(push switch)
		- 비밀번호 입력 시 입력한 숫자가 한자리씩 출력된다 (dot)
		- 네자리 입력 완료 시 전체 숫자가 출력된다 (fnd)
- 비밀번호 일치 시 문이 열리고 닫힌다. (step motor)
- 비밀번호 불일치 시 불일치 메시지가 출력된다.(text lcd)
- 초기 비밀번호 입력 대기 상태로 돌아간다.

## 3. 기능별 세부 설계
![image](https://github.com/user-attachments/assets/e3c6e474-93cb-4c7f-a48d-c9130bdfdca0)   
먼저 dip switch 시작 기능이다.   
text LCD 로 “Welcome to Home! Pull dip switch” 메시지를 출력후   
while(1)의 무한루프를 1초마다  돌며 dip_switch의 값을 read한다.   
dip switch의 1번 스위치가 내려간 값을 나타내는 0x01이 read되면   
무한루프를 탈출하여 다음 프로세스를 진행한다.
   
![image](https://github.com/user-attachments/assets/fe6c0ef9-fd77-4c3b-9f3c-89e86def1dc9)   
다음은 비밀번호 입력이다. text LCD 를 통해 “please press password” 메시지가 출력되고   
프로그램은 while(1)을 통해 계속해서 push switch의 값을 read한다.   
push switch는 0~8의 아홉개의 버튼중 하나가 눌리면   
눌린 동안 0으로 채워진 int[8]의 배열 중 해당 인덱스를 1로 바꾸어 놓으므로   
push switch 의 리드값을 for( 0~8 )로 검사하여 인덱스의 값이 바뀌었는지를 검사한다.   
바뀌었으면 버튼이 눌린 것이므로 4자리 배열에 하나씩 저장한다.   
dot device에 입력된 숫자를 출력한다.   
   
이때 push switch의 버튼을 누르고 떼기 전에 코드상 처리가 끝나고 다음 처리가 또 일어나   
같은 숫자가 입력되는 일이 발생하기 때문에   
숫자 하나가 입력되면 sleep(1)을 통해 다음 입력 전까지의 처리시간을 확보한다.   
   	
네개의 숫자가 입력되면 fnd 에 입력된 네자리 숫자를 출력하고 다음 프로세스로 진행한다.
   
![image](https://github.com/user-attachments/assets/10fa1794-f44d-4d52-a143-cf70810c1bde)   
네자리 숫자가 입력되면 코드상 저장된 비밀번호 네자리와 비교한다.
	
for( 0~3 )으로 비교하여 불일치 시   
text LCD 로 “wrong password” 메시지를 출력하고,   
LED 를 1초간 on – off  총 3번 반복하여 경고를 표시한다.   
		
text LCD 의 표시 결과를 “ “로 초기화 후 for문을 탈출한다.   
   
비밀번호 재입력을 위해 while(1) 내에서 프로그램이 동작하고 있으므로   
dot와 fnd를 0과 0000으로 초기화 출력 후  continue; 를 통하여 비밀번호 입력 대기 상태로 복귀한다.

![image](https://github.com/user-attachments/assets/e74e585b-57f2-45e8-be73-524f2c48b12a)   
입력된 숫자가 비밀번호와 일치한다면  위의 비밀번호 불일치 검사를 지나오게 된다.
   
step motor를 반시계 방향으로 4초 회전하고, (문열림)   
sleep(3) 잠시 멈춘다. (문 통과 대기)   
step motor를 시계 방향으로 4초 회전한다. (문 닫힘)   
   
문 열림-닫힘 프로세스가 종료되면 while(1)문의 끝을 만나 다시 비밀번호 입력대기 상태로 복귀한다.

<br>

## 4. 부록(소스코드)
<br>
소스코드는 다음 파일에서 열람하실 수 있습니다.   

[Smart_Door_Lock.c](./Smart_Door_Lock.c)
