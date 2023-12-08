import { CUSTOM_ELEMENTS_SCHEMA, Component } from '@angular/core';
import { MatCardModule } from '@angular/material/card';

@Component({
  selector: 'app-navbar',
  standalone: true,
  imports: [MatCardModule],
  schemas:[CUSTOM_ELEMENTS_SCHEMA],
  templateUrl: './navbar.component.html',
  styleUrl: './navbar.component.css'
})
export class NavbarComponent {

}
