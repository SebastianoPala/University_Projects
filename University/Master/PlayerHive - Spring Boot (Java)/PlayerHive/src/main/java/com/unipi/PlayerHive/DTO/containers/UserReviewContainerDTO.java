package com.unipi.PlayerHive.DTO.containers;

import com.unipi.PlayerHive.DTO.reviews.UserReviewDTO;
import lombok.AllArgsConstructor;
import lombok.Getter;

import java.util.List;

@Getter
@AllArgsConstructor
public class UserReviewContainerDTO {

    private List<UserReviewDTO> reviews;

    private int numPages;

    private boolean isLastPage;
}
