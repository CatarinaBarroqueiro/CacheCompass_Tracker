import { Input, Component, Output, EventEmitter, ViewChild, ElementRef } from '@angular/core';
import { FormGroup, FormControl } from '@angular/forms';
import { MaterialModule } from '../../material/material.module';
import { MatDialog } from '@angular/material/dialog';
import { DialogComponent } from '../../dialog/dialog.component';
import { HttpClient, HttpClientModule } from '@angular/common/http';
import { Router, RouterModule } from '@angular/router';


@Component({
  selector: 'app-log-in',
  standalone: true,
  imports: [HttpClientModule, RouterModule],
  templateUrl: './log-in.component.html',
  styleUrl: './log-in.component.css'
})


export class LogInComponent {
  constructor(
    private dialog: MatDialog,
    private http: HttpClient,
    private router: Router
  ) {}

  @ViewChild('email') emailInput!: ElementRef<HTMLInputElement>;
  @ViewChild('password') passInput!: ElementRef<HTMLInputElement>;

  form: FormGroup = new FormGroup({
    email: new FormControl(''),
    password: new FormControl('')
  });

  verify() {
    const userEmail = this.emailInput.nativeElement.value;
    const userPassword = this.passInput.nativeElement.value;

    this.http.get<any>('http://51.20.64.70:3000/user').subscribe((response) => {
      for (let j = 0; j < response.data.length; j++) {
        const user = response.data[j];
        if (userEmail === user.email && userPassword === user.password) {
          // Both email and password match, navigate to /history
          this.router.navigate(['/history']);
          return;
        }
      }

      // Email or password doesn't match, display an error message and emit an event
      console.log('Invalid email or password');
      this.openDialog('Invalid email or password. Please try again.');
      this.submitEM.emit(false); // Emit event to inform parent component about failure
    });
  }
  

  openDialog(message: string): void {
    this.dialog.open(DialogComponent, {
      data: { message },
    });
  }

  @Input() error: string | null | undefined;
  @Output() submitEM = new EventEmitter();
}